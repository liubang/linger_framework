/*
  +----------------------------------------------------------------------+
  | linger_framework                                                     |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: liubang <it.liubang@gmail.com>                               |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "Zend/zend_interfaces.h"
#include "SAPI.h"

#include "php_linger_framework.h"
#include "linger_dispatcher.h"
#include "linger_controller.h"
#include "linger_request.h"
#include "linger_router.h"
#include "linger_router_rule.h"
#include "linger_response.h"

ZEND_BEGIN_ARG_INFO_EX(linger_framework_dispatcher_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

zend_class_entry *dispatcher_ce;

zval *linger_dispatcher_instance(zval *this, zval *router)
{
    zval *instance = zend_read_static_property(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_INSTANCE), 1);

    if (Z_TYPE_P(instance) == IS_OBJECT &&
            instanceof_function(Z_OBJCE_P(instance), dispatcher_ce TSRMLS_CC)) {
        return instance;
    }

    if (Z_ISUNDEF_P(this)) {
        object_init_ex(this, dispatcher_ce);
    }

    zend_update_static_property(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_INSTANCE), instance);

    zval request = {{0}};
    (void)linger_request_instance(&request);

    if (EXPECTED(Z_TYPE(request) == IS_OBJECT)) {
        zend_update_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_REQUEST), &request);
        zval_ptr_dtor(&request);
    } else {
        linger_throw_exception(NULL, 0, "request must be a instance of linger\\framework\\Request.");
        zval_ptr_dtor(this);
        return NULL;
    }

    if (EXPECTED(router && IS_OBJECT == Z_TYPE_P(router)
                 && instanceof_function(Z_OBJCE_P(router), router_ce))) {
        zend_update_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_ROUTER), router);
    } else {
        linger_throw_exception(NULL, 0, "router must be a instance of linger\\framework\\Router.");
        zval_ptr_dtor(this);
        return NULL;
    }

    return this;
}

void linger_dispatcher_dispatch(zval *this)
{
    zval *request = zend_read_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_REQUEST), 1, NULL);
    zval *router = zend_read_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_ROUTER),1, NULL);
    zval *router_rule;

    if ((router_rule = linger_router_match(router, request)) == NULL) {
        _404();
        return;
    }

    zval *class = linger_router_rule_get_class(router_rule);
    zval *class_method = linger_router_rule_get_class_method(router_rule);

    if (EXPECTED(class && class_method)) {
        // ZEND_API zend_class_entry *zend_lookup_class(zend_string *name);
        zend_class_entry *ce;
        if ((ce = zend_lookup_class(Z_STR(*class))) == NULL) {
            linger_throw_exception(NULL, 0, "class %s is not exists.", Z_STRVAL_P(class));
            return;
        }

        zval controller_obj = {{0}};
        zval **fptr;
        object_init_ex(&controller_obj, ce);
        if (linger_controller_construct(ce, &controller_obj, request) == FAILURE)
            return;

        char *class_method_lower = zend_str_tolower_dup(Z_STRVAL_P(class_method), Z_STRLEN_P(class_method));
        zend_string *zs_class_method = zend_string_init(class_method_lower, Z_STRLEN_P(class_method), 0);
        if (zend_hash_exists(&((ce)->function_table), zs_class_method)) {
            zend_call_method(&controller_obj, ce, NULL, class_method_lower, Z_STRLEN_P(class_method), NULL, 0, NULL, NULL);
        } else {
            linger_throw_exception(NULL, 0, "the method %s of %s is not exists.", Z_STRVAL_P(class_method),
                                   Z_STRVAL_P(class));
        }

        linger_efree(class_method_lower);
        zend_string_release(zs_class_method);
        zval_ptr_dtor(&controller_obj);
    } else {
        linger_throw_exception(NULL, 0, "invalid request.");
    }
}

PHP_METHOD(linger_framework_dispatcher, __construct)
{

}

PHP_METHOD(linger_framework_dispatcher, findRouter)
{
    zval *request = zend_read_property(dispatcher_ce, getThis(), ZEND_STRL(DISPATCHER_PROPERTIES_REQUEST), 1, NULL);
    zval *router = zend_read_property(dispatcher_ce, getThis(), ZEND_STRL(DISPATCHER_PROPERTIES_ROUTER),1, NULL);
    zval *router_rule = linger_router_match(router, request);
    if (NULL != router_rule) {
        RETURN_ZVAL(router_rule, 1, 0);
    } else {
        RETURN_NULL();
    }
}

PHP_METHOD(linger_framework_dispatcher, getRequest)
{
    zval *request = zend_read_property(dispatcher_ce, getThis(), ZEND_STRL(DISPATCHER_PROPERTIES_REQUEST), 1, NULL);

    RETURN_ZVAL(request, 1, 0);
}

zend_function_entry dispatcher_methods[] = {
    PHP_ME(linger_framework_dispatcher, __construct, NULL, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_dispatcher, findRouter, linger_framework_dispatcher_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_dispatcher, getRequest, linger_framework_dispatcher_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(dispatcher)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\Dispatcher", dispatcher_methods);
    dispatcher_ce = zend_register_internal_class_ex(&ce, NULL);
    zend_declare_property_null(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_INSTANCE), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);
    zend_declare_property_null(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_ROUTER), ZEND_ACC_PRIVATE);
    zend_declare_property_null(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_REQUEST), ZEND_ACC_PRIVATE);
    return SUCCESS;
}
