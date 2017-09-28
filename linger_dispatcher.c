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
#include "php_linger_framework.h"
#include "linger_request.h"

zend_class_entry *dispatcher_ce;
zend_class_entry *request_ce;

#define DISPATCHER_PROPERTIES_INSTANCE "_instance"
#define DISPATCHER_PROPERTIES_REQUEST  "_request"
#define DISPATCHER_PROPERTIES_MODULE "_module"
#define DISPATCHER_PROPERTIES_CONTROLLER "_controller"
#define DISPATCHER_PROPERTIES_ACTION "_action"

zval *linger_dispatcher_instance(zval *this, zval *request TSRMLS_DC) {
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
            zend_throw_exception(NULL, "request must be a instance of linger_framework_Request", 0 TSRMLS_CC);
            return;
        }
    }
    return instance;
}

static void linger_dispatcher_prepare(zval *this TSRMLS_DC) {
    if (this == NULL) {
        zend_throw_exception(NULL, "null pointer exception", 0 TSRMLS_CC);
        return;
    }
    zval *request = zend_read_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_REQUEST), 1 TSRMLS_CC); 
    if (Z_TYPE_P(request) == IS_OBJECT) {
        char *uri = linger_request_get_request_uri(request);
        if (uri == NULL) {
            zend_throw_exception(NULL, "illegal access!", 0 TSRMLS_CC); 
            return;
        }
        char *copy = estrdup(uri);
        char *mvc;
        zval *module;
        zval *controller;
        zval *action;
        MAKE_STD_ZVAL(module);
        MAKE_STD_ZVAL(controller);
        MAKE_STD_ZVAL(action);
        mvc = strtok(copy, "/"); 
        if (mvc != NULL) {
            ZVAL_STRING(module, mvc, 1);
        } else {
            goto end;
        }
        mvc = strtok(NULL, "/");
        if (mvc != NULL) {
            ZVAL_STRING(controller, mvc, 1);
        } else {
            goto end;
        }
        mvc = strtok(NULL, "/");
        if (mvc != NULL) {
            ZVAL_STRING(action, mvc, 1);
        } else {
            goto end;
        }
end:
        if (Z_TYPE_P(module) == IS_NULL) {
            ZVAL_STRING(module, "index", 1);
        }
        if (Z_TYPE_P(controller) == IS_NULL) {
            ZVAL_STRING(controller, "index", 1);
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
        zend_throw_exception(NULL, "illegal arrtribute", 0 TSRMLS_CC);
        return;
    }
}

//TODO
static int linger_dispatcher_auto_load() {
    return 1;
}

#define LINGER_FRAMEWORK_MODULE_DIR_NAME        "module"
#define LINGER_FRAMEWORK_CONTROLLER_DIR_NAME    "controller"
#define LINGER_FRAMEWORK_VIEW_DIR_NAME          "view"

static zend_class_entry *linger_dispatcher_get_controller(char *app_dir, char *module, char *controller TSRMLS_DC) {
    char *directory = NULL;
    int directory_len = 0;
    directory_len = spprintf(&directory, 0, "%s%c%s%c%s%c%s", app_dir, '/', LINGER_FRAMEWORK_MODULE_DIR_NAME, '/', module, '/', LINGER_FRAMEWORK_CONTROLLER_DIR_NAME);
    if (directory_len) {
        char *class = NULL;
        char *class_lowercase = NULL;
        int class_len = 0;
        zend_class_entry **ce = NULL;
        class_len = spprintf(&class, 0, "%s%s", controller, "Controller");
        class_lowercase = zend_str_tolower_dup(class, class_len);
        if (zend_hash_find(EG(class_table), class_lowercase, class_len + 1, (void **)&ce) != SUCCESS) {
            //TODO autoload 
            return;
        }
        linger_efree(class);
        linger_efree(class_lowercase);
        linger_efree(directory);
        return *ce;
    }
    return NULL;
}

void linger_dispatcher_dispatch(zval *this TSRMLS_DC) {
    if (this != NULL) {
        linger_dispatcher_prepare(this TSRMLS_CC);
        zval *module, *controller, *action;
        module = zend_read_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_MODULE), 1 TSRMLS_CC);
        controller = zend_read_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_CONTROLLER), 1 TSRMLS_CC);
        action = zend_read_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_ACTION), 1 TSRMLS_CC);
        if (Z_TYPE_P(module) != IS_STRING || Z_TYPE_P(controller) != IS_STRING || Z_TYPE_P(action) != IS_STRING) {
            zend_throw_exception(NULL, "illegal access", 0 TSRMLS_CC);
            return;
        }

        zend_class_entry *ce = linger_dispatcher_get_controller("app", Z_STRVAL_P(module), Z_STRVAL_P(controller) TSRMLS_CC);
        if (!ce) {
            return;
        }
        zval *icontroller;
        MAKE_STD_ZVAL(icontroller);
        object_init_ex(icontroller, ce);
        if (!linger_controller_construct(ce, icontroller)) {
            return; 
        }
        char *action_lower = zend_str_tolower_dup(Z_STRVAL_P(action), Z_STRLEN_P(action));
        char *func_name;
        int func_name_len;
        func_name_len = spprintf(&func_name, 0, "%s%s", action_lower, "action");
        linger_efree(action_lower);
        zval **fptr, *ret;
        if (zend_hash_find(&((ce)->function_table), func_name, func_name_len + 1, (void **)&fptr) == SUCCESS) {
            zend_call_method(&icontroller, ce, NULL, func_name, func_name_len, &ret, 0, NULL, NULL TSRMLS_CC);
            zval_ptr_dtor(&ret);
        }
        linger_efree(func_name);
    } else {
        zend_throw_exception(NULL, "null pointer exception", 0 TSRMLS_CC);
        return;
    }
}

PHP_METHOD(linger_framework_dispatcher, __construct)
{

}

PHP_METHOD(linger_framework_dispatcher, getRequest)
{
    zval *request = zend_read_property(dispatcher_ce, getThis(), ZEND_STRL(DISPATCHER_PROPERTIES_REQUEST), 1 TSRMLS_CC);
    RETURN_ZVAL(request, 1, 0);
}

zend_function_entry dispatcher_methods[] = {
    PHP_ME(linger_framework_dispatcher, __construct, NULL, ZEND_ACC_PROTECTED | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_dispatcher, getRequest, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(dispatcher)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\Dispatcher", dispatcher_methods);
    dispatcher_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_INSTANCE), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC TSRMLS_CC);
    zend_declare_property_null(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_MODULE), ZEND_ACC_PRIVATE);
    zend_declare_property_null(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_REQUEST), ZEND_ACC_PRIVATE);
    zend_declare_property_null(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_CONTROLLER), ZEND_ACC_PRIVATE);
    zend_declare_property_null(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_ACTION), ZEND_ACC_PRIVATE);
    return SUCCESS;
}
