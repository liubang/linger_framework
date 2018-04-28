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
#include "php_linger_framework.h"
#include "ext/standard/php_string.h"
#include "linger_router_rule.h"

zend_class_entry *router_rule_ce;

ZEND_BEGIN_ARG_INFO_EX(linger_framework_router_rule_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(linger_framework_router_rule_4_arginfo, 0, 0, 4)
ZEND_ARG_INFO(0, request_method)
ZEND_ARG_INFO(0, uri)
ZEND_ARG_INFO(0, class)
ZEND_ARG_INFO(0, class_method)
ZEND_END_ARG_INFO()

zend_object_handlers  linger_router_rule_obj_handlers;

static zend_object *linger_create_router_rule_obj(zend_class_entry *ce) /* {{{ */
{
    router_rule_obj *internal;
    internal = ecalloc(1, sizeof(router_rule_obj) + zend_object_properties_size(ce));

    zend_object_std_init(&internal->std, ce);
    object_properties_init(&internal->std, ce);
    internal->std.handlers = &linger_router_rule_obj_handlers;
    internal->zs_uri = NULL;

    return &internal->std;
}
/* }}} */

static void linger_free_router_rule_obj(zend_object *object) /* {{{ */
{
    router_rule_obj *internal = (router_rule_obj *)((char *)object - XtOffsetOf(router_rule_obj, std));

    zend_object_std_dtor(&internal->std);

    if (internal->zs_uri) {
        zend_string_release(internal->zs_uri);
    }
} /* }}} */

zval *linger_router_rule_instance(zval *this, zval *request_method, zval *uri, zval *class, zval *class_method)  /* {{{ */
{
    if (Z_ISUNDEF_P(this)) {
        object_init_ex(this, router_rule_ce);
    }

    char *lower_method = zend_str_tolower_dup(Z_STRVAL_P(request_method), Z_STRLEN_P(request_method));
    if (!strncmp(lower_method, "get", 3)
            || !strncmp(lower_method, "post", 4)
            || !strncmp(lower_method, "put", 3)
            || !strncmp(lower_method, "delete", 6)) {
        zend_update_property_string(router_rule_ce, this, ZEND_STRL(ROUTER_RULE_PROPERTIES_REQUEST_METHOD), lower_method);
    } else {
        linger_throw_exception(NULL, 0, "invalid http request.");
        linger_efree(lower_method);
        return NULL;
    }

    if (UNEXPECTED(IS_STRING != Z_TYPE_P(uri) || IS_STRING != Z_TYPE_P(request_method)
                   || IS_STRING != Z_TYPE_P(class) || IS_STRING != Z_TYPE_P(class_method))) {
        linger_throw_exception(NULL, 0, "parameters error.");
        linger_efree(lower_method);
        return NULL;
    }

    zend_update_property_string(router_rule_ce, this, ZEND_STRL(ROUTER_RULE_PROPERTIES_URI), Z_STRVAL_P(uri));
    zend_update_property(router_rule_ce, this, ZEND_STRL(ROUTER_RULE_PROPERTIES_CLASS), class);
    zend_update_property(router_rule_ce, this, ZEND_STRL(ROUTER_RULE_PROPERTIES_CLASS_METHOD), class_method);
    linger_efree(lower_method);

    //Z_GET_ZS_URI(this) = zend_string_init(Z_STRVAL_P(uri), Z_STRLEN_P(uri), 0);

    return this;
}
/* }}} */

zval *linger_router_rule_get_request_method(zval *this) /* {{{ */ 
{
    if (!this)
        return NULL;
    return zend_read_property(router_rule_ce, this, ZEND_STRL(ROUTER_RULE_PROPERTIES_REQUEST_METHOD), 1, NULL);
}
/* }}} */

zval *linger_router_rule_get_uri(zval *this) /* {{{ */ 
{
    if (!this)
        return NULL;
    return zend_read_property(router_rule_ce, this, ZEND_STRL(ROUTER_RULE_PROPERTIES_URI), 1, NULL);
}
/* }}} */

zval *linger_router_rule_get_class(zval *this) /* {{{ */ 
{
    if (!this)
        return NULL;
    return zend_read_property(router_rule_ce, this, ZEND_STRL(ROUTER_RULE_PROPERTIES_CLASS), 1, NULL);
}
/* }}} */

zval *linger_router_rule_get_class_method(zval *this) /* {{{ */ 
{
    if (UNEXPECTED(!this))
        return NULL;

    return zend_read_property(router_rule_ce, this, ZEND_STRL(ROUTER_RULE_PROPERTIES_CLASS_METHOD), 1, NULL);
}
/* }}} */

int linger_router_rule_set_compiled_uri(zval *this, zend_string *compiled_uri) /* {{{ */ 
{
    if (UNEXPECTED(!this || !compiled_uri)) 
        return FAILURE;

    zend_string *old = Z_GET_ZS_URI(this);

    if (old) {
        zend_string_release(old);
    }

    Z_GET_ZS_URI(this) = compiled_uri;

    return SUCCESS;
}
/* }}} */

zend_string *linger_router_rule_get_compiled_uri(zval *this) /* {{{ */
{
    if (UNEXPECTED(!this))
        return NULL;

    return Z_GET_ZS_URI(this);
}
/* }}} */

int linger_router_rule_set_params_map(zval *this, zval *params_map) /* {{{ */ 
{
    if (!this || IS_ARRAY != Z_TYPE_P(params_map))
        return FAILURE;

    zend_update_property(router_rule_ce, this, ZEND_STRL(ROUTER_RULE_PROPERTIES_PARAMS_MAP), params_map);

    return SUCCESS;
}
/* }}} */

zval *linger_router_rule_get_params_map(zval *this) /* {{{ */
{
    if (!this)
        return NULL;

    return zend_read_property(router_rule_ce, this, ZEND_STRL(ROUTER_RULE_PROPERTIES_PARAMS_MAP), 1, NULL);
}
/* }}} */

PHP_METHOD(linger_framework_router_rule, __construct) /* {{{ */ 
{
    zval *request_method, *uri, *class, *class_method;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zzzz",
                                    &request_method, &uri, &class, &class_method) == FAILURE) {
        return;
    }

    (void)linger_router_rule_instance(getThis(), request_method, uri, class, class_method);
}
/* }}} */

PHP_METHOD(linger_framework_router_rule, getRequestMethod) /* {{{ */
{
    zval *request_method = zend_read_property(router_rule_ce, getThis(), ZEND_STRL(ROUTER_RULE_PROPERTIES_REQUEST_METHOD), 1, NULL);

    RETURN_ZVAL(request_method, 1, 0);
}
/* }}} */

PHP_METHOD(linger_framework_router_rule, getUri) /* {{{ */ 
{
    zval *uri = zend_read_property(router_rule_ce, getThis(), ZEND_STRL(ROUTER_RULE_PROPERTIES_URI), 1, NULL);

    RETURN_ZVAL(uri, 1, 0);
}
/* }}} */

PHP_METHOD(linger_framework_router_rule, getClass) /* {{{ */
{
    zval *class = zend_read_property(router_rule_ce, getThis(), ZEND_STRL(ROUTER_RULE_PROPERTIES_CLASS), 1, NULL);

    RETURN_ZVAL(class, 1, 0);
}
/* }}} */

PHP_METHOD(linger_framework_router_rule, getClassMethod) /* {{{ */
{
    zval *class_method = zend_read_property(router_rule_ce, getThis(), ZEND_STRL(ROUTER_RULE_PROPERTIES_CLASS_METHOD), 1, NULL);

    RETURN_ZVAL(class_method, 1, 0);
}
/* }}} */

zend_function_entry router_rule_methods[] = { /* {{{ */ 
    PHP_ME(linger_framework_router_rule, __construct, linger_framework_router_rule_4_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_router_rule, getRequestMethod, linger_framework_router_rule_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router_rule, getUri, linger_framework_router_rule_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router_rule, getClass, linger_framework_router_rule_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router_rule, getClassMethod, linger_framework_router_rule_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
};
/* }}} */

LINGER_MINIT_FUNCTION(router_rule) /* {{{ */
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\RouterRule", router_rule_methods);
    router_rule_ce = zend_register_internal_class_ex(&ce, NULL);
    router_rule_ce->ce_flags |= ZEND_ACC_FINAL;
    router_rule_ce->create_object = linger_create_router_rule_obj;

    memcpy(&linger_router_rule_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    linger_router_rule_obj_handlers.offset = XtOffsetOf(router_rule_obj, std);
    linger_router_rule_obj_handlers.dtor_obj = zend_objects_destroy_object;
    linger_router_rule_obj_handlers.free_obj = linger_free_router_rule_obj;

    zend_declare_property_null(router_rule_ce, ZEND_STRL(ROUTER_RULE_PROPERTIES_REQUEST_METHOD), ZEND_ACC_PRIVATE);
    zend_declare_property_null(router_rule_ce, ZEND_STRL(ROUTER_RULE_PROPERTIES_URI), ZEND_ACC_PRIVATE);
    zend_declare_property_null(router_rule_ce, ZEND_STRL(ROUTER_RULE_PROPERTIES_PARAMS_MAP), ZEND_ACC_PRIVATE);
    zend_declare_property_null(router_rule_ce, ZEND_STRL(ROUTER_RULE_PROPERTIES_CLASS), ZEND_ACC_PRIVATE);
    zend_declare_property_null(router_rule_ce, ZEND_STRL(ROUTER_RULE_PROPERTIES_CLASS_METHOD), ZEND_ACC_PRIVATE);
    return SUCCESS;
}
/* }}} */

