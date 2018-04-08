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
#include "php_linger_framework.h"
#include "SAPI.h"

#include "linger_dispatcher.h"
#include "linger_controller.h"
#include "linger_request.h"
#include "linger_router.h"
#include "linger_router_rule.h"

#define LINGER_FRAMEWORK_MODULE_DIR_NAME        "module"
#define LINGER_FRAMEWORK_CONTROLLER_DIR_NAME    "controller"
#define LINGER_FRAMEWORK_VIEW_DIR_NAME          "view"

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

void linger_dispatcher_prepare(zval *this)
{
    if (!this)
        return;

    zval *request = zend_read_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_REQUEST), 1, NULL);

    if (UNEXPECTED(!request || IS_OBJECT != Z_TYPE_P(request)))
        return;

    zval *uri = linger_request_get_request_uri(request);

    char *copy = estrdup(Z_STRVAL_P(uri));
    char *mvc;
    zval module = {{0}},
    controller = {{0}},
    action = {{0}};
    char *ptrptr;

#define STRTOK(s, d, v)   \
	do { \
		mvc = php_strtok_r(s, d, &ptrptr); \
		if (mvc) { \
			ZVAL_STRING(v, mvc); \
		} else {\
			goto end; \
		} \
	} while(0)

    STRTOK(copy, "/", &module);
    STRTOK(NULL, "/", &controller);
    STRTOK(NULL, "/", &action);

    if (NULL != copy) {
        char *key;
        char *val;
        while ((key = php_strtok_r(NULL, "/", &ptrptr))
                && (val = php_strtok_r(NULL, "/", &ptrptr))) {
            linger_request_set_param(request, key, val);
        }
    }

end:
    if (Z_ISUNDEF(module)) {
        ZVAL_STRING(&module, "index");
    }

    if (Z_ISUNDEF(controller)) {
        ZVAL_STRING(&controller, "index");
    } else {
        // PHPAPI char *php_strtoupper(char *s, size_t len)
        zend_string_release(Z_STR(controller));
        ZVAL_STRINGL(&controller, php_strtoupper(Z_STRVAL(controller), Z_STRLEN(controller) + 1), Z_STRLEN(controller));
    }

    if (Z_ISUNDEF(action)) {
        ZVAL_STRING(&action, "index");
    }

    zend_update_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_MODULE), &module);
    zend_update_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_CONTROLLER), &controller);
    zend_update_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_ACTION), &action);
    zval_ptr_dtor(&module);
    zval_ptr_dtor(&controller);
    zval_ptr_dtor(&action);
    linger_efree(copy);
}

static zend_class_entry *linger_dispatcher_get_controller(char *app_dir, char *module, char *controller)
{
    char *directory = NULL;
    int directory_len = 0;

    directory_len = spprintf(&directory, 0, "%s%c%s%c%s%c%s", app_dir, DEFAULT_SLASH, LINGER_FRAMEWORK_MODULE_DIR_NAME,
                             DEFAULT_SLASH, module, DEFAULT_SLASH, LINGER_FRAMEWORK_CONTROLLER_DIR_NAME);

    if (!directory_len)
        return NULL;

    char *class = NULL;
    char *class_lowercase = NULL;
    int class_len = 0;
    zval *zce = NULL;

    class_len = spprintf(&class, 0, "%s%s", controller, "Controller");
    class_lowercase = zend_str_tolower_dup(class, class_len);

    zend_string *zs_class_lowercase = zend_string_init(class_lowercase, class_len, 0);

    if ((zce = zend_hash_find(EG(class_table), zs_class_lowercase)) == NULL)  {
        char *controller_path = NULL;
        int controller_path_len = 0;
        controller_path_len = spprintf(&controller_path, 0, "%s%c%s%s", directory, DEFAULT_SLASH, controller, ".php");

        linger_efree(directory);

        if (linger_framework_include_scripts(controller_path, controller_path_len, NULL) == SUCCESS) {
            linger_efree(controller_path);
            if ((zce = zend_hash_find(EG(class_table), zs_class_lowercase)) == NULL) {
                linger_throw_exception(NULL, 0, "could not find class %s.", class);
                linger_efree(class);
                linger_efree(class_lowercase);
                zend_string_release(zs_class_lowercase);
                return NULL;
            }
        } else {
            linger_throw_exception(NULL, 0, "failed opening script %s.", controller_path);
            linger_efree(class);
            linger_efree(class_lowercase);
            linger_efree(controller_path);
            zend_string_release(zs_class_lowercase);
            return NULL;
        }
    }

    linger_efree(LINGER_FRAMEWORK_G(view_directory));
    char *view_directory = NULL;
    (void)spprintf(&view_directory, 0, "%s%c%s%c%s%c%s", app_dir, DEFAULT_SLASH, LINGER_FRAMEWORK_MODULE_DIR_NAME,
                   DEFAULT_SLASH, module, DEFAULT_SLASH, LINGER_FRAMEWORK_VIEW_DIR_NAME);
    LINGER_FRAMEWORK_G(view_directory) = view_directory;

    zend_string_release(zs_class_lowercase);
    linger_efree(class);
    linger_efree(class_lowercase);
    linger_efree(directory);

    return Z_CE_P(zce);
}

#define _404() \
	do { \
		sapi_header_line ctr = { \
			.line = "HTTP/1.1 404 Not Found", \
			.line_len = 22, \
			.response_code = 404}; \
		sapi_header_op(SAPI_HEADER_REPLACE, &ctr TSRMLS_CC); \
		php_printf("<h1>404 Not Found</h1>"); \
	} while (0)

void linger_dispatcher_dispatch(zval *this)
{
    zval *request = zend_read_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_REQUEST), 1, NULL);
    zval *router = zend_read_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_ROUTER),1, NULL);
    zval *router_rule;

    if ((router_rule = linger_router_match(router, request)) == NULL) {
        _404();
        return;
    }

    linger_dispatcher_prepare(this);

    zval *class = linger_router_rule_get_class(router_rule);
    zval *class_method = linger_router_rule_get_class_method(router_rule);

    if (EXPECTED(class && class_method)) {
        // ZEND_API zend_class_entry *zend_lookup_class(zend_string *name);
        zend_class_entry *ce;
        if ((ce = zend_lookup_class(Z_STR(*class))) == NULL) {
            linger_throw_exception(NULL, 0, "class %s does not exists.", Z_STRVAL_P(class));
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
            linger_throw_exception(NULL, 0, "the method %s of controller %s is not exists.", Z_STRVAL_P(class_method),
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
    zend_declare_property_null(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_MODULE), ZEND_ACC_PRIVATE);
    zend_declare_property_null(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_REQUEST), ZEND_ACC_PRIVATE);
    zend_declare_property_null(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_CONTROLLER), ZEND_ACC_PRIVATE);
    zend_declare_property_null(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_ACTION), ZEND_ACC_PRIVATE);
    return SUCCESS;
}
