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
#include "Zend/zend_interfaces.h"
#include "php_linger_framework.h"
#include "linger_request.h"
#include "linger_router.h"
#include "linger_router_rule.h"

zend_class_entry *dispatcher_ce;
zend_class_entry *request_ce;

#define DISPATCHER_PROPERTIES_INSTANCE "_instance"
#define DISPATCHER_PROPERTIES_REQUEST  "_request"
#define DISPATCHER_PROPERTIES_ROUTER   "_router"
#define DISPATCHER_PROPERTIES_MODULE "_module"
#define DISPATCHER_PROPERTIES_CONTROLLER "_controller"
#define DISPATCHER_PROPERTIES_ACTION "_action"

zval *linger_dispatcher_instance(zval *this, zval *request TSRMLS_DC)
{
    zval *instance = zend_read_static_property(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_INSTANCE), 1 TSRMLS_CC);
    if (Z_TYPE_P(instance) == IS_OBJECT &&
            instanceof_function(Z_OBJCE_P(instance), dispatcher_ce)) {
        return instance;
    }
    if (this) {
        instance = this;
    } else {
        instance = NULL;
        MAKE_STD_ZVAL(instance);
        object_init_ex(instance, dispatcher_ce);
    }
    zend_update_static_property(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_INSTANCE), instance TSRMLS_CC);
    if (request != NULL) {
        if (Z_TYPE_P(request) == IS_OBJECT &&
                instanceof_function(Z_OBJCE_P(request), request_ce)) {
            zend_update_property(dispatcher_ce, instance, ZEND_STRL(DISPATCHER_PROPERTIES_REQUEST), request TSRMLS_CC);
        } else {
            linger_throw_exception(NULL, 0, "request must be a instance of linger_framework_Request.");
            return NULL;
        }
    }
    zval *router = linger_router_instance(NULL TSRMLS_CC);
    zend_update_property(dispatcher_ce, instance, ZEND_STRL(DISPATCHER_PROPERTIES_ROUTER), router TSRMLS_CC);
    return instance;
}

#define MAKE_STD_ZVAL_NULL(z)  \
    do {                       \
        MAKE_STD_ZVAL(z);      \
        ZVAL_NULL(z);          \
    } while(0)

#define STRTOK(s, d, v)                     \
    do {                                    \
        mvc = php_strtok_r(s, d, &ptrptr);  \
        if (mvc) {                          \
            ZVAL_STRING(v, mvc, 1);         \
        } else {                            \
            goto end;                       \
        }                                   \
    } while(0)

static void linger_dispatcher_prepare(zval *this TSRMLS_DC)
{
    if (this == NULL) {
        return;
    }
    zval *request = zend_read_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_REQUEST), 1 TSRMLS_CC);
    if (Z_TYPE_P(request) == IS_OBJECT) {
        zval *uri = linger_request_get_request_uri(request TSRMLS_CC);
        if (uri == NULL) {
            linger_throw_exception(NULL, 0, 'illegal access.');
            return;
        }
        char *copy = estrdup(Z_STRVAL_P(uri));
        char *mvc;
        zval *module;
        zval *controller;
        zval *action;

        MAKE_STD_ZVAL_NULL(module);
        MAKE_STD_ZVAL_NULL(controller);
        MAKE_STD_ZVAL_NULL(action);

        char *ptrptr;
        STRTOK(copy, "/", module);
        STRTOK(NULL, "/", controller);
        STRTOK(NULL, "/", action);

        //param
        if (copy != NULL) {
            char *key = NULL;
            char *val = NULL;
            key = php_strtok_r(NULL, "/", &ptrptr);
            val = php_strtok_r(NULL, "/", &ptrptr);
            while(key && val) {
                linger_request_set_param(request, key, val TSRMLS_CC);
                key = php_strtok_r(NULL, "/", &ptrptr);
                val = php_strtok_r(NULL, "/", &ptrptr);
            }
        }

end:
        if (Z_TYPE_P(module) == IS_NULL) {
            ZVAL_STRING(module, "index", 1);
        }
        if (Z_TYPE_P(controller) == IS_NULL) {
            ZVAL_STRING(controller, "Index", 1);
        } else {
            *(Z_STRVAL_P(controller)) = toupper(*(Z_STRVAL_P(controller)));
        }
        if (Z_TYPE_P(action) == IS_NULL) {
            ZVAL_STRING(action, "index", 1);
        }
        zend_update_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_MODULE), module TSRMLS_CC);
        zend_update_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_CONTROLLER), controller TSRMLS_CC);
        zend_update_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_ACTION), action TSRMLS_CC);
        zval_ptr_dtor(&module);
        zval_ptr_dtor(&controller);
        zval_ptr_dtor(&action);
        linger_efree(copy);
    } else {
        linger_throw_exception(NULL, 0, 'illegal arrtribute.');
        return;
    }
}

#define LINGER_FRAMEWORK_MODULE_DIR_NAME        "module"
#define LINGER_FRAMEWORK_CONTROLLER_DIR_NAME    "controller"
#define LINGER_FRAMEWORK_VIEW_DIR_NAME          "view"

static zend_class_entry *linger_dispatcher_get_controller(char *app_dir, char *module, char *controller TSRMLS_DC)
{
    char *directory = NULL;
    int directory_len = 0;
    directory_len = spprintf(&directory, 0, "%s%c%s%c%s%c%s", app_dir, DEFAULT_SLASH, LINGER_FRAMEWORK_MODULE_DIR_NAME, DEFAULT_SLASH, module, DEFAULT_SLASH, LINGER_FRAMEWORK_CONTROLLER_DIR_NAME);
    if (directory_len) {
        char *class = NULL;
        char *class_lowercase = NULL;
        int class_len = 0;
        zend_class_entry **ce = NULL;
        class_len = spprintf(&class, 0, "%s%s", controller, "Controller");
        class_lowercase = zend_str_tolower_dup(class, class_len);
        if (zend_hash_find(EG(class_table), class_lowercase, class_len + 1, (void **)&ce) != SUCCESS) {
            char *controller_path = NULL;
            int controller_path_len = 0;
            controller_path_len = spprintf(&controller_path, 0, "%s%c%s%s", directory, DEFAULT_SLASH, controller, ".php");
            if (linger_application_import(controller_path, controller_path_len + 1, 0 TSRMLS_CC) == FAILURE) {
                linger_efree(controller_path);
                linger_efree(class);
                linger_efree(class_lowercase);
                linger_efree(directory);
                linger_throw_exception(NULL, 0, "failed opening script %s.", controller_path);
                return NULL;
            } else {
                if (zend_hash_find(EG(class_table), class_lowercase, class_len + 1, (void **)&ce) != SUCCESS) {
                    linger_efree(class);
                    linger_efree(class_lowercase);
                    linger_efree(directory);
                    linger_throw_exception(NULL, 0, "could not find class %s.", class);
                    return NULL;
                }
            }
        }
        linger_efree(LINGER_FRAMEWORK_G(view_directory));
        char *view_directory = NULL;
        (void)spprintf(&view_directory, 0, "%s%c%s%c%s%c%s", app_dir, DEFAULT_SLASH, LINGER_FRAMEWORK_MODULE_DIR_NAME, DEFAULT_SLASH, module, DEFAULT_SLASH, LINGER_FRAMEWORK_VIEW_DIR_NAME);
        LINGER_FRAMEWORK_G(view_directory) = view_directory;
        linger_efree(class);
        linger_efree(class_lowercase);
        linger_efree(directory);
        return *ce;
    }
    return NULL;
}

void linger_dispatcher_dispatch(zval *this TSRMLS_DC)
{
    if (this != NULL) {
        linger_dispatcher_prepare(this TSRMLS_CC);
        zval *module, *controller, *action;
        module = zend_read_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_MODULE), 1 TSRMLS_CC);
        controller = zend_read_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_CONTROLLER), 1 TSRMLS_CC);
        action = zend_read_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_ACTION), 1 TSRMLS_CC);
        zval *request = zend_read_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_REQUEST), 1 TSRMLS_CC);
        if (Z_TYPE_P(module) != IS_STRING || Z_TYPE_P(controller) != IS_STRING || Z_TYPE_P(action) != IS_STRING) {
            linger_throw_exception(NULL, 0, "illegal access.");
            return;
        }

        zend_class_entry *ce = linger_dispatcher_get_controller(LINGER_FRAMEWORK_G(app_directory), Z_STRVAL_P(module), Z_STRVAL_P(controller) TSRMLS_CC);
        if (!ce) {
            linger_throw_exception(NULL, 0, "class %sController is not exists.", Z_STRVAL_P(controller));
            return;
        }
        zval *icontroller;
        MAKE_STD_ZVAL(icontroller);
        object_init_ex(icontroller, ce);
        if (FAILURE == linger_controller_construct(ce, icontroller, request TSRMLS_CC)) {
            return;
        }
        char *action_lower = zend_str_tolower_dup(Z_STRVAL_P(action), Z_STRLEN_P(action));
        char *func_name;
        int func_name_len;
        func_name_len = spprintf(&func_name, 0, "%s%s", action_lower, "action");
        linger_efree(action_lower);
        zval **fptr, *ret = NULL;
        if (zend_hash_find(&((ce)->function_table), func_name, func_name_len + 1, (void **)&fptr) == SUCCESS) {
            zend_call_method(&icontroller, ce, NULL, func_name, func_name_len, &ret, 0, NULL, NULL TSRMLS_CC);
            if (ret) zval_ptr_dtor(&ret);
        } else {
            linger_throw_exception(NULL, 0, "the method %sAction of controller %s is not exists.", Z_STRVAL_P(action), Z_STRVAL_P(controller));
            return;
        }
        linger_efree(func_name);
    } else {
        return;
    }
}

void linger_dispatcher_dispatch_ex(zval *this TSRMLS_DC)
{
    zval *request = zend_read_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_REQUEST), 1 TSRMLS_CC);
    zval *router = zend_read_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_ROUTER),1 TSRMLS_CC);
    zval *router_rule;
    if ((router_rule = linger_router_match(router, request TSRMLS_CC)) == NULL) {
        return;
    }
    zval *class = linger_router_rule_get_class(router_rule TSRMLS_CC);
    zval *class_method = linger_router_rule_get_class_method(router_rule TSRMLS_CC);
    if (NULL != class && NULL != class_method) {
        zend_class_entry **ce;
        if (zend_lookup_class(Z_STRVAL_P(class), Z_STRLEN_P(class), &ce TSRMLS_CC) != SUCCESS) {
            linger_throw_exception(NULL, 0, "class %s does not exists.", Z_STRVAL_P(class));
            return;
        }
        zval *controller_obj;
        zval **fptr;
        MAKE_STD_ZVAL(controller_obj);
        object_init_ex(controller_obj, *ce);
        if (FAILURE == linger_controller_construct(*ce, controller_obj, request TSRMLS_CC)) {
            return;
        }
        if (zend_hash_find(&((*ce)->function_table), Z_STRVAL_P(class_method), Z_STRLEN_P(class_method) + 1, (void **)&fptr) == SUCCESS) {
            zend_call_method_with_0_params(&controller_obj, *ce, NULL, Z_STRVAL_P(class_method), NULL);
        }
    }
    return;
}

PHP_METHOD(linger_framework_dispatcher, __construct)
{
}

PHP_METHOD(linger_framework_dispatcher, findRouter)
{
    zval *request = zend_read_property(dispatcher_ce, getThis(), ZEND_STRL(DISPATCHER_PROPERTIES_REQUEST), 1 TSRMLS_CC);
    zval *router = zend_read_property(dispatcher_ce, getThis(), ZEND_STRL(DISPATCHER_PROPERTIES_ROUTER),1 TSRMLS_CC);
    //zval *uri = linger_request_get_request_uri(request TSRMLS_CC);
    //zval *request_method = linger_request_get_request_method(request TSRMLS_CC);
    //TODO params check.
    zval *router_rule = linger_router_match(router, request TSRMLS_CC);
    if (NULL != router_rule) {
        RETURN_ZVAL(router_rule, 1, 0);
    } else {
        RETURN_FALSE;
    }
}

PHP_METHOD(linger_framework_dispatcher, getRequest)
{
    zval *request = zend_read_property(dispatcher_ce, getThis(), ZEND_STRL(DISPATCHER_PROPERTIES_REQUEST), 1 TSRMLS_CC);
    RETURN_ZVAL(request, 1, 0);
}

zend_function_entry dispatcher_methods[] = {
    PHP_ME(linger_framework_dispatcher, __construct, NULL, ZEND_ACC_PROTECTED | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_dispatcher, getRequest, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_dispatcher, findRouter, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(dispatcher)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\Dispatcher", dispatcher_methods);
    dispatcher_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_INSTANCE), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC TSRMLS_CC);
    zend_declare_property_null(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_ROUTER), ZEND_ACC_PRIVATE);
    zend_declare_property_null(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_MODULE), ZEND_ACC_PRIVATE);
    zend_declare_property_null(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_REQUEST), ZEND_ACC_PRIVATE);
    zend_declare_property_null(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_CONTROLLER), ZEND_ACC_PRIVATE);
    zend_declare_property_null(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_ACTION), ZEND_ACC_PRIVATE);
    return SUCCESS;
}
