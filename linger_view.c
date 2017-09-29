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

zend_class_entry *view_ce;

#define VIEW_PROPERTIES_VARS "_vars"

zval *linger_view_instance(TSRMLS_DC)
{
    zval *instance = NULL;
    MAKE_STD_ZVAL(instance);
    object_init_ex(instance, view_ce);
    zval *vars = NULL;
    MAKE_STD_ZVAL(vars);
    array_init(vars);
    zend_update_property(view_ce, instance, ZEND_STRL(VIEW_PROPERTIES_VARS), vars TSRMLS_CC);
    zval_ptr_dtor(&vars);
    return instance;
}

int linger_view_assign(zval *this, zval *key, zval *val TSRMLS_DC)
{
    if (!this || !key || !val) {
        return FAILURE;
    }
    zval *vars = zend_read_property(view_ce, this, ZEND_STRL(VIEW_PROPERTIES_VARS), 1 TSRMLS_CC);
    add_assoc_zval(vars, key, val);
    zend_update_property(view_ce, this, ZEND_STRL(VIEW_PROPERTIES_VARS), vars TSRMLS_CC);
    return SUCCESS;
}

zval *linger_view_getVars(zval *this TSRMLS_DC)
{
    if (!this) {
        return NULL;
    } 
    zval *vars = zend_read_property(view_ce, this, ZEND_STRL(VIEW_PROPERTIES_VARS), 1 TSRMLS_CC);
    return vars;
}

PHP_METHOD(linger_framework_view, __construct)
{

}

PHP_METHOD(linger_framework_view, assign)
{
    char *key = NULL;
    uint key_len;
    zval *val = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &key, &key_len, &val) == FAILURE) {
        return;
    }
    if (key_len > 0 && val != NULL) {
        if (linger_view_assign(getThis(), key, val) == FAILURE) {
            zend_throw_exception(NULL, "assign variable fail");
            return;
        }
        Z_ADDREF_P(val);
    }
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(linger_framework_view, display)
{

}

PHP_METHOD(linger_framework_view, render)
{

}

PHP_METHOD(linger_framework_view, getVars)
{
    zval *vars = linger_view_getVars(getThis() TSRMLS_CC);
    RETURN_ZVAL(vars, 1, 0);
}

zend_function_entry view_methods[] = {
    PHP_ME(linger_framework_view, __construct, NULL, ZEND_ACC_PROTECTED | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_view, assign, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_view, display, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_view, render, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_view, getVars, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(view)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\View", view_methods);
    view_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(view_ce, ZEND_STRL(VIEW_PROPERTIES_VARS), ZEND_ACC_PROTECTED);
    return SUCCESS;
}
