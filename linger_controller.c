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

int linger_controller_construct(zend_class_entry *ce, zval *this TSRMLS_DC) {

    if (!instanceof_function(ce, controller_ce)) {
        zend_throw_exception(NULL, "controller must be a instance of linger_framework_Controller");
        return 0;
    }
    // call _init method
    if (zend_hash_exists(&(ce->function_table), ZEND_STRS("_init"))) {
        zend_call_method_with_0_params(&this, ce, NULL, "_init", NULL);
    }
}

PHP_METHOD(linger_framework_controller, __construct)
{

}

PHP_METHOD(linger_framework_controller, _init)
{

}

zend_function_entry controller_methods[] = {
    PHP_ME(linger_framework_controller, __construct, NULL, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_controller, _init, NULL, ZEND_ACC_PROTECTED)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(controller)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\Controller", controller_methods);
    controller_ce = zend_register_internal_class(&ce TSRMLS_CC);
    return SUCCESS;
}

