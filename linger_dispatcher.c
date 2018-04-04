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

    }

}
