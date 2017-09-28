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
#include "linger_controller.h"
zend_class_entry *controller_ce;

#define CONTROLLER_PROPERTIES_REQUEST   "_request"
#define CONTROLLER_PROPERTIES_RESPONSE  "_response"

int linger_controller_construct(zend_class_entry *ce, zval *this, zval *request TSRMLS_DC)
{
    if (!instanceof_function(ce, controller_ce)) {
        zend_throw_exception(NULL, "controller must be a instance of linger_framework_Controller");
        return 0;
    }
    zend_update_property(controller_ce, this, ZEND_STRL(CONTROLLER_PROPERTIES_REQUEST), request TSRMLS_CC);
    // call _init method
    if (zend_hash_exists(&(ce->function_table), ZEND_STRS("_init"))) {
        zend_call_method_with_0_params(&this, ce, NULL, "_init", NULL);
    }
    return 1;
}

PHP_METHOD(linger_framework_controller, __construct)
{

}

PHP_METHOD(linger_framework_controller, _init)
{

}

PHP_METHOD(linger_framework_controller, getRequest)
{
    zval *request = zend_read_property(controller_ce, getThis(), ZEND_STRL(CONTROLLER_PROPERTIES_REQUEST), 1 TSRMLS_CC);
    RETURN_ZVAL(request, 1, 0);
}

zend_function_entry controller_methods[] = {
    PHP_ME(linger_framework_controller, __construct, NULL, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_controller, _init, NULL, ZEND_ACC_PROTECTED)
    PHP_ME(linger_framework_controller, getRequest, NULL, ZEND_ACC_PROTECTED)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(controller)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\Controller", controller_methods);
    controller_ce = zend_register_internal_class(&ce TSRMLS_CC);
    controller_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;
    zend_declare_property_null(controller_ce, ZEND_STRL(CONTROLLER_PROPERTIES_REQUEST), ZEND_ACC_PROTECTED);
    zend_declare_property_null(controller_ce, ZEND_STRL(CONTROLLER_PROPERTIES_RESPONSE), ZEND_ACC_PROTECTED);
    return SUCCESS;
}

