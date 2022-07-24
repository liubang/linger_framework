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
#include "Zend/zend_interfaces.h" // zend_call_method_with_1_params
#include "php_linger_framework.h"
#include "linger_application.h"
#include "linger_config.h"
#include "linger_router.h"
#include "linger_dispatcher.h"
#include "linger_bootstrap.h"

#if PHP_MAJOR_VERSION > 7
#include "linger_application_arginfo.h"
#else
#include "linger_application_legacy_arginfo.h"
#endif

/* class entry */
zend_class_entry *application_ce;

PHP_METHOD(linger_framework_application, autoload)
{
    zend_string *class_name;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "S", &class_name) == FAILURE) {
        return;
    }

    char class_path[1024]; 
    char c;

    int i = 0,
        j = 0;

    while ((c = *(LINGER_FRAMEWORK_G(app_directory) + i)) != '\0') {
        class_path[i] = c;
        i++;
    }

    class_path[i] = DEFAULT_SLASH;
    i++;

    while((c = *(ZSTR_VAL(class_name) + j)) != '\0') {
        if (c == '\\')
            c = DEFAULT_SLASH;

        class_path[i + j] = c;
        j++;
    }

    if (i + j + 4 > 1023) {
        linger_throw_exception(NULL, 0, "the class_path %s is too long.", class_path);
        return;
    }

    class_path[i + j] = '.';
    class_path[i + j + 1] = 'p';
    class_path[i + j + 2] = 'h';
    class_path[i + j + 3] = 'p';
    class_path[i + j + 4] = '\0';

    if (linger_framework_include_scripts(class_path, i + j, NULL) != SUCCESS) {
        linger_throw_exception(NULL, 0, "the file %s is not exists.", class_path);
        return;
    }
}

PHP_METHOD(linger_framework_application, __construct)
{
    zval *app,
         *aconfig = NULL,
         zconfig = {{0}};

    app = zend_read_static_property(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_APP), 1);

    if (!ZVAL_IS_NULL(app)) {
        linger_throw_exception(NULL, 0, "can not reinstance application.");
        return;
    }

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &aconfig) == FAILURE) {
        return;
    }

    if (!aconfig || Z_TYPE_P(aconfig) != IS_ARRAY) {
        linger_throw_exception(NULL, 0, "config must be an array.");
        return;
    }

    // app_directory
    zend_string *zs_app_directory = zend_string_init("app_directory", 13, 0);
    zval *zv_app_directory = NULL;
    if ((zv_app_directory = zend_hash_find(Z_ARRVAL_P(aconfig), zs_app_directory)) == NULL) {
        linger_throw_exception(NULL, 0, "must set app_directory in config.");
        zend_string_release(zs_app_directory);
        return;
    }
    zend_string_release(zs_app_directory);

    if (*(Z_STRVAL_P(zv_app_directory) + Z_STRLEN_P(zv_app_directory) - 1) == DEFAULT_SLASH) {
        LINGER_FRAMEWORK_G(app_directory) = estrndup(Z_STRVAL_P(zv_app_directory), Z_STRLEN_P(zv_app_directory) - 1);
    } else {
        LINGER_FRAMEWORK_G(app_directory) = estrndup(Z_STRVAL_P(zv_app_directory), Z_STRLEN_P(zv_app_directory));
    }

    // view_directory
    zend_string *zs_view_directory = zend_string_init("view_directory", 14, 0);
    zval *zv_view_directory = NULL;
    if ((zv_view_directory = zend_hash_find(Z_ARRVAL_P(aconfig), zs_view_directory)) != NULL) {
        if (*(Z_STRVAL_P(zv_view_directory) + Z_STRLEN_P(zv_view_directory) - 1) == DEFAULT_SLASH) {
            LINGER_FRAMEWORK_G(view_directory) = estrndup(Z_STRVAL_P(zv_view_directory), Z_STRLEN_P(zv_view_directory) - 1);
        } else {
            LINGER_FRAMEWORK_G(view_directory) = estrndup(Z_STRVAL_P(zv_view_directory), Z_STRLEN_P(zv_view_directory));
        }
    }
    zend_string_release(zs_view_directory);

    zval zv_autoload = {{0}};
    ZVAL_STRING(&zv_autoload, "\\linger\\framework\\Application::autoload");
    zend_call_method_with_1_params(NULL, NULL, NULL, "spl_autoload_register", NULL, &zv_autoload);
    zval_ptr_dtor(&zv_autoload);

    zval *self = getThis();
    zend_update_static_property(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_APP), self);

    // init config
    (void)linger_config_instance(&zconfig, aconfig);
    zend_update_property(application_ce, Z_OBJ_P(self), ZEND_STRL(APPLICATION_PROPERTIES_CONFIG), &zconfig);
    zval_ptr_dtor(&zconfig);

    // init router
    zval zrouter = {{0}};
    (void)linger_router_instance(&zrouter);

    // init dispatcher
    zval zdispatcher = {{0}};
    (void)linger_dispatcher_instance(&zdispatcher, &zrouter);
    zend_update_property(application_ce, Z_OBJ_P(self), ZEND_STRL(APPLICATION_PROPERTIES_DISPATCHER), &zdispatcher);
    zend_update_property(application_ce, Z_OBJ_P(self), ZEND_STRL(APPLICATION_PROPERTIES_ROUTER), &zrouter);
    zval_ptr_dtor(&zrouter);
    zval_ptr_dtor(&zdispatcher);
}

PHP_METHOD(linger_framework_application, init)
{
    zval *bootclasses = NULL;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &bootclasses) == FAILURE) {
        return;
    }

    if (IS_ARRAY != Z_TYPE_P(bootclasses)) {
        linger_throw_exception(NULL, 0, "the parameter must be an array.");
        return;
    }

    zend_array *hash = HASH_OF(bootclasses);
    HashPosition pos;
    zval method = {{0}};
    ZVAL_STRING(&method, "bootstrap");
    for (zend_hash_internal_pointer_reset_ex(hash, &pos);
            zend_hash_has_more_elements_ex(hash, &pos) == SUCCESS;
            zend_hash_move_forward_ex(hash, &pos)) {
        zval *pzval;
        if ((pzval = zend_hash_get_current_data_ex(hash, &pos)) == NULL) {
            continue;
        }
        if (IS_STRING != Z_TYPE_P(pzval)) {
            continue;
        }
        zend_class_entry *ce;
        if ((ce = zend_lookup_class(Z_STR_P(pzval))) == NULL) {
            linger_throw_exception(NULL, 0, "class %s dose not exists.", Z_STRVAL_P(pzval));
            RETURN_FALSE;
        }
        zval boot_obj = {{0}};
        zval **fptr;
        object_init_ex(&boot_obj, ce);
        if (!instanceof_function(Z_OBJCE(boot_obj), bootstrap_ce)) {
            linger_throw_exception(NULL, 0, "class %s must be subclass of %s.", Z_STRVAL_P(pzval), bootstrap_ce->name->val);
            continue;
        }
        if (zend_hash_find(&(ce->function_table), Z_STR(method)) != NULL) {
            zend_call_method_with_1_params(Z_OBJ_P(&boot_obj), ce, NULL, "bootstrap", NULL, getThis());
        }
        zval_ptr_dtor(&boot_obj);
    }

    zval_ptr_dtor(&method);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(linger_framework_application, run)
{
    zval *dispatcher = zend_read_property(application_ce, Z_OBJ_P(getThis()), ZEND_STRL(APPLICATION_PROPERTIES_DISPATCHER), 1, NULL);
    linger_dispatcher_dispatch(dispatcher);
}

PHP_METHOD(linger_framework_application, app)
{
    zval *app = zend_read_static_property(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_APP), 1);
    RETURN_ZVAL(app, 1, 0);
}

PHP_METHOD(linger_framework_application, getConfig)
{
    zval *config = zend_read_property(application_ce, Z_OBJ_P(getThis()), ZEND_STRL(APPLICATION_PROPERTIES_CONFIG), 1, NULL);
    RETURN_ZVAL(config, 1, 0);
}

PHP_METHOD(linger_framework_application, getRouter)
{
    zval *router = zend_read_property(application_ce, Z_OBJ_P(getThis()), ZEND_STRL(APPLICATION_PROPERTIES_ROUTER), 1, NULL);
    RETURN_ZVAL(router, 1, 0);
}

PHP_METHOD(linger_framework_application, getDispatcher)
{
    zval *dispatcher = zend_read_property(application_ce, Z_OBJ_P(getThis()), ZEND_STRL(APPLICATION_PROPERTIES_DISPATCHER), 1, NULL);
    RETURN_ZVAL(dispatcher, 1, 0);
}

PHP_METHOD(linger_framework_application, __destruct)
{
    zend_update_static_property_null(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_APP));
}

zend_function_entry application_methods[] = {
    PHP_ME(linger_framework_application, autoload, arginfo_class_Linger_Framework_Application_autoload, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(linger_framework_application, __construct, arginfo_class_Linger_Framework_Application___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_application, app, arginfo_class_Linger_Framework_Application_app, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(linger_framework_application, init, arginfo_class_Linger_Framework_Application_init, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_application, run, arginfo_class_Linger_Framework_Application_run,ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_application, getConfig, arginfo_class_Linger_Framework_Application_getConfig, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_application, getRouter, arginfo_class_Linger_Framework_Application_getRouter, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_application, getDispatcher, arginfo_class_Linger_Framework_Application_getDispatcher, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(application)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\Application", application_methods);
    application_ce = zend_register_internal_class_ex(&ce, NULL);

#if PHP_VERSION_ID < 80100
    application_ce->ce_flags |= ZEND_ACC_FINAL;
	application_ce->serialize = zend_class_serialize_deny;
	application_ce->unserialize = zend_class_unserialize_deny;
#else
	application_ce->ce_flags |= ZEND_ACC_FINAL | ZEND_ACC_NOT_SERIALIZABLE;
#endif

    zend_declare_property_null(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_APP), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);
    zend_declare_property_null(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_CONFIG), ZEND_ACC_PROTECTED);
    zend_declare_property_null(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_ROUTER), ZEND_ACC_PROTECTED);
    zend_declare_property_null(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_DISPATCHER), ZEND_ACC_PROTECTED);

    return SUCCESS;
}
