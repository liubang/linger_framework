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
#include "SAPI.h"

#include "linger_request.h"
#include "linger_router.h"
#include "linger_router_rule.h"

#define LINGER_FRAMEWORK_MODULE_DIR_NAME        "module"
#define LINGER_FRAMEWORK_CONTROLLER_DIR_NAME    "controller"
#define LINGER_FRAMEWORK_VIEW_DIR_NAME          "view"

zend_class_entry *dispatcher_ce;

zval *linger_dispatcher_instance(zval *this, zval *request, zval *router)
{
    zval *instance = zend_read_static_property(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_INSTANCE), 1, NULL);

    if (Z_TYPE_P(instance) == IS_OBJECT &&
            instanceof_function(Z_OBJCE_P(instance), dispatcher_ce TSRMLS_CC)) {
        return instance;
    }

    if (Z_ISUNDEF_P(this)) {
        object_init_ex(this, dispatcher_ce);
    }

    zend_update_static_property(dispatcher_ce, ZEND_STRL(DISPATCHER_PROPERTIES_INSTANCE), instance);

    if (EXPECTED(request && IS_OBJECT == Z_TYPE_P(request)
                 && instanceof_function(Z_OBJCE_P(request), request_ce))) {
        zend_update_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_REQUEST), request);
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

static void linger_dispatcher_prepare(zval *this)
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
        while ((key = php_strtok_r(NULL, "/", &ptrptr))
                && (val = php_strtok_r(NULL, "/", &ptrptr))) {
            linger_request_set_param(request, key, val);
        }
    }

end:
    if (Z_ISUNDEF(module))
        ZVAL_STRING(&module, "index");
    if (Z_ISUNDEF(controller))
        ZVAL_STRING(&controller, "index");
    else
        Z_STRVAL(controller) = toupper(Z_STRVAL(controller));

    if (Z_ISUNDEF(action))
        ZVAL_STRING(&action, "index");

    zend_update_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_MODULE), module);
    zend_update_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_CONTROLLER), controller);
    zend_update_property(dispatcher_ce, this, ZEND_STRL(DISPATCHER_PROPERTIES_ACTION), action);
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

    if ((zce = zend_hash_find(EG(class_table), class_lowercase, class_len + 1)) == NULL)  {
        char *controller_path = NULL;
        int controller_path_len = 0;
        controller_path_len = spprintf(&controller_path, 0, "%s%c%s%s", directory, DEFAULT_SLASH, controller, ".php");

        linger_efree(directory);

        if (linger_framework_include_scripts(controller_path, controller_path_len + 1, NULL) == SUCCESS) {
            linger_efree(controller_path);
            if ((zce = zend_hash_find(EG(class_table), class_lowercase, class_len + 1)) == NULL) {
                linger_throw_exception(NULL, 0, "could not find class %s.", class);
                linger_efree(class);
                linger_efree(class_lowercase);
                return NULL;
            }
        } else {
            linger_throw_exception(NULL, 0, "failed opening script %s.", controller_path);
            linger_efree(class);
            linger_efree(class_lowercase);
            linger_efree(controller_path);
            return NULL;
        }
    }

    linger_efree(LINGER_FRAMEWORK_G(view_directory));
    char *view_directory = NULL;
    (void)spprintf(&view_directory, 0, "%s%c%s%c%s%c%s", app_dir, DEFAULT_SLASH, LINGER_FRAMEWORK_MODULE_DIR_NAME,
                   DEFAULT_SLASH, module, DEFAULT_SLASH, LINGER_FRAMEWORK_VIEW_DIR_NAME);
    LINGER_FRAMEWORK_G(view_directory) = view_directory;

    linger_efree(class);
    linger_efree(class_lowercase);
    linger_efree(directory);

    return Z_CE_P(zce);
}

void linger_dispatcher_dispatch_ex(zval *this)
{

}
