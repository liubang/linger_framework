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

zend_class_entry *router_ce;

#define ROUTER_PROPERTIES_RULS  "_ruls"

zend_function_entry router_methods[] = {
    PHP_FE_END
};

PHP_MINIT_FUNCTION(router)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(&ce, "Linger\\Framework\\Router", router_methods);
    router_ce = zend_register_internal_class(ce TSRMLS_CC);
    zend_declare_property_null(router_ce, ZEND_STRL(ROUTER_PROPERTIES_RULS), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);
    return SUCCESS;
}
