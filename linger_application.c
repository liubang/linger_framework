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
#include "php_linger_framework.h"
#include "linger_config.h"
#include "linger_router.h"
#include "linger_request.h"
#include "linger_dispatcher.h"
#include "linger_response.h"

zend_class_entry *application_ce;
zend_class_entry *config_ce;

#define MAXPATHLEN                        1024
#define APPLICATION_PROPERTIES_APP        "_app"
#define APPLICATION_PROPERTIES_CONFIG     "_config"
#define APPLICATION_PROPERTIES_ROUTER     "_router"
#define APPLICATION_PROPERTIES_REQUEST    "_request"
#define APPLICATION_PROPERTIES_RESPONSE   "_response"
#define APPLICATION_PROPERTIES_DISPATCHER "_dispatcher"

#define STORE_EG_ENVIRON() \
{ \
    zval **__old_return_value_pp = EG(return_value_ptr_ptr); \
    zend_op **__old_opline_ptr   = EG(opline_ptr); \
    zend_op_array *__old_op_array= EG(active_op_array);

#define RESTORE_EG_ENVIRON() \
    EG(return_value_ptr_ptr)     = __old_return_value_pp;\
    EG(opline_ptr)               = __old_opline_ptr;\
    EG(active_op_array)          = __old_op_array;\
}

int linger_application_import(char *path, int len, int use_path TSRMLS_DC)
{
    zend_file_handle file_handle;
    zend_op_array *op_array;
    char *realpath[MAXPATHLEN];
    if (!VCWD_REALPATH(path, realpath)) {
        return FAILURE;
    }

    file_handle.filename = path;
    file_handle.free_filename = 0;
    file_handle.type = ZEND_HANDLE_FILENAME;
    file_handle.opened_path = NULL;
    file_handle.handle.fp = NULL;

    op_array = zend_compile_file(&file_handle, ZEND_INCLUDE TSRMLS_CC);

    if (op_array && file_handle.handle.stream.handle) {
        int dummy = 1;
        if (!file_handle.opened_path) {
            file_handle.opened_path = path;
        }
        zend_hash_add(&EG(included_files), file_handle.opened_path, strlen(file_handle.opened_path) + 1, (void **)&dummy, sizeof(int), NULL);
    }
    zend_destroy_file_handle(&file_handle TSRMLS_CC);

    if (op_array) {
        zval *result = NULL;
        STORE_EG_ENVIRON();

        EG(return_value_ptr_ptr) = &result;
        EG(active_op_array) = op_array;
#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION > 2) || (PHP_MAJOR_VERSION > 5))
        if (!EG(active_symbol_table)) {
#if PHP_MINOR_VERSION < 5
            zval *orig_this = EG(This);
            EG(This) = NULL;
            zend_rebuild_symbol_table(TSRMLS_C);
            EG(This) = orig_this;
#else
            zend_rebuild_symbol_table(TSRMLS_C);
#endif
        }
#endif
        zend_execute(op_array TSRMLS_CC);
        destroy_op_array(op_array TSRMLS_CC);
        linger_efree(op_array);
        if (!EG(exception)) {
            if (EG(return_value_ptr_ptr) && *EG(return_value_ptr_ptr)) {
                zval_ptr_dtor(EG(return_value_ptr_ptr));
            }
        }
        RESTORE_EG_ENVIRON();
        return SUCCESS;
    }

    return FAILURE;
}

PHP_METHOD(linger_framework_application, __construct)
{
    zval *oconfig, *aconfig = NULL;
    zval *orouter;
    zval *orequest;
    zval *oresponse;
    zval *odispatcher;
    zval *app;

    app = zend_read_static_property(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_APP), 1 TSRMLS_CC);

    if (!ZVAL_IS_NULL(app)) {
        zend_throw_exception(NULL, "Can not reinstance application", 0 TSRMLS_CC);
        RETURN_FALSE;
    }

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &aconfig) == FAILURE) {
        return;
    }

    if (aconfig && Z_TYPE_P(aconfig) != IS_ARRAY) {
        zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "config must be array");
        RETURN_FALSE;
    }
    zval *self = getThis();
    oconfig = linger_config_instance(NULL, aconfig TSRMLS_CC);
    zend_update_property(application_ce, self, ZEND_STRL(APPLICATION_PROPERTIES_CONFIG), oconfig TSRMLS_CC);

    orequest = linger_request_instance(NULL, NULL TSRMLS_CC);
    odispatcher = linger_dispatcher_instance(NULL, orequest TSRMLS_CC);
    orouter = linger_router_instance(NULL TSRMLS_CC);
    zend_update_property(application_ce, self, ZEND_STRL(APPLICATION_PROPERTIES_DISPATCHER), odispatcher TSRMLS_CC);
    zend_update_property(application_ce, self, ZEND_STRL(APPLICATION_PROPERTIES_REQUEST), orequest TSRMLS_CC);
    zend_update_property(application_ce, self, ZEND_STRL(APPLICATION_PROPERTIES_ROUTER), orouter TSRMLS_CC);
    zend_update_static_property(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_APP), self TSRMLS_CC);

    zval_ptr_dtor(&oconfig);
    zval_ptr_dtor(&orequest);
    zval_ptr_dtor(&odispatcher);

    HashTable *conf = Z_ARRVAL_P(aconfig);
    zval **ppzval;
    if (zend_hash_find(conf, ZEND_STRS("app_directory"), (void **)&ppzval) == FAILURE) {
        zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "must set app_directory in config");
        RETURN_FALSE;
    }

    if (*(Z_STRVAL_PP(ppzval) + Z_STRLEN_PP(ppzval) - 1) == DEFAULT_SLASH) {
        LINGER_FRAMEWORK_G(app_directory) = estrndup(Z_STRVAL_PP(ppzval), Z_STRLEN_PP(ppzval) - 1);
    } else {
        LINGER_FRAMEWORK_G(app_directory) = estrndup(Z_STRVAL_PP(ppzval), Z_STRLEN_PP(ppzval));
    }
}

PHP_METHOD(linger_framework_application, bootstrap)
{
    zval *bootclasses = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &bootclasses) == FAILURE) {
        return;
    }
    if (IS_ARRAY != Z_TYPE_P(bootclasses)) {
        zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "the parameter must be an array.");
        RETURN_FALSE;
    }
    HashTable *hash = HASH_OF(bootclasses); 
    HashPosition pos;
    for (zend_hash_internal_pointer_reset_ex(hash, &pos); 
            zend_hash_has_more_elements_ex(hash, &pos) == SUCCESS; 
            zend_hash_move_forward_ex(hash, &pos)) {
        zval **ppzval;
        if (zend_hash_get_current_data_ex(arrht,(void**)&ppzval, &pos) == FAILURE) {
            continue;
        }
        if (IS_STRING != Z_TYPE_PP(ppzval)) {
            continue;
        } 
        zend_class_entry **ce;
        if (zend_hash_find(EG(class_table), Z_STRVAL_PP(ppzval), Z_STRLEN_PP(ppzval), (void **)&ce) != SUCCESS) {
            zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "class %s not exists.", Z_STRVAL_PP(ppzval));
            RETURN_FALSE;
        }
        //
    }
}

PHP_METHOD(linger_framework_application, run)
{
    //dispatcher dispatche.
    zval *dispatcher = zend_read_property(application_ce, getThis(), ZEND_STRL(APPLICATION_PROPERTIES_DISPATCHER), 1 TSRMLS_CC);
    linger_dispatcher_dispatch(dispatcher TSRMLS_CC);
}

PHP_METHOD(linger_framework_application, app)
{
    //static method to get app instance.
    zval *app = zend_read_static_property(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_APP), 1 TSRMLS_CC);
    RETURN_ZVAL(app, 1, 0);
}

PHP_METHOD(linger_framework_application, getConfig)
{
    zval *config = zend_read_property(application_ce, getThis(), ZEND_STRL(APPLICATION_PROPERTIES_CONFIG), 1 TSRMLS_CC);
    RETURN_ZVAL(config, 1, 0);
}

PHP_METHOD(linger_framework_application, getRouter)
{
    zval *router = zend_read_property(application_ce, getThis(), ZEND_STRL(APPLICATION_PROPERTIES_ROUTER), 1 TSRMLS_CC);
    RETURN_ZVAL(router, 1, 0);
}

PHP_METHOD(linger_framework_application, getDispatcher)
{
    zval *dispatcher = zend_read_property(application_ce, getThis(), ZEND_STRL(APPLICATION_PROPERTIES_DISPATCHER), 1 TSRMLS_CC);
    RETURN_ZVAL(dispatcher, 1, 0);
}

PHP_METHOD(linger_framework_application, getRequest)
{
    zval *request = zend_read_property(application_ce, getThis(), ZEND_STRL(APPLICATION_PROPERTIES_REQUEST), 1 TSRMLS_CC);
    RETURN_ZVAL(request, 1, 0);
}

PHP_METHOD(linger_framework_application, setConfig)
{
    zval *config;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &config) == FAILURE) {
        return;
    }
    if (Z_TYPE_P(config) == IS_OBJECT &&
            instanceof_function(Z_OBJCE_P(config), config_ce)) {
        zend_update_property(application_ce, getThis(), ZEND_STRL(APPLICATION_PROPERTIES_CONFIG), config TSRMLS_CC);
        RETURN_ZVAL(getThis(), 1, 0);
    } else {
        zend_throw_exception(NULL, "config must be a instance of linger_framework_Config", 0 TSRMLS_CC);
        return;
    }
}

PHP_METHOD(linger_framework_application, __destruct)
{
    zend_update_static_property_null(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_APP));
}

zend_function_entry application_methods[] = {
    PHP_ME(linger_framework_application, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_application, run, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_application, app, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(linger_framework_application, getConfig, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_application, getRouter, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_application, setConfig, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_application, getDispatcher, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_application, getRequest, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_application, __destruct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(application)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\Application", application_methods);
    application_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_APP), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);
    zend_declare_property_null(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_CONFIG), ZEND_ACC_PROTECTED);
    zend_declare_property_null(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_ROUTER), ZEND_ACC_PROTECTED);
    zend_declare_property_null(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_REQUEST), ZEND_ACC_PROTECTED);
    zend_declare_property_null(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_DISPATCHER), ZEND_ACC_PROTECTED);
    return SUCCESS;
}
