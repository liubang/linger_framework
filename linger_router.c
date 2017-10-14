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
#include "linger_router_rule.h"

zend_class_entry *router_ce;

#define LINGER_ROUTER_PROPERTIES_INSTANCE "_instance"
#define LINGER_ROUTER_PROPERTIES_RULES  "_rules"

zval *linger_router_instance(zval *this TSRMLS_DC)
{
    zval *instance = zend_read_static_property(router_ce, ZEND_STRL(LINGER_ROUTER_PROPERTIES_INSTANCE), 1 TSRMLS_CC);
    if (Z_TYPE_P(instance) == IS_OBJECT &&
            instanceof_function(Z_OBJCE_P(instance), router_ce)) {
        return instance;
    }
    if (this) {
        instance = this;
    } else {
        instance = NULL;
        MAKE_STD_ZVAL(instance);
        object_init_ex(instance, router_ce);
    }
    zend_update_static_property(router_ce, ZEND_STRL(LINGER_ROUTER_PROPERTIES_INSTANCE), instance TSRMLS_CC);
    zval *rules;
    MAKE_STD_ZVAL(rules);
    array_init(rules);
    zend_update_property(router_ce, instance, ZEND_STRL(LINGER_ROUTER_PROPERTIES_RULES), rules TSRMLS_CC);
    zval_ptr_dtor(&rules);
    return instance;
}

zval *linger_router_match(zval *dispatcher_obj, zval *this, char *uri TSRMLS_DC) {

}

PHP_METHOD(linger_framework_router, __construct)
{

}

PHP_METHOD(linger_framework_router, add)
{
    char *uri;
    char *class;
    char *class_method;
    uint uri_len;
    uint class_len;
    uint class_method_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sss", &uri, &uri_len, &class, &class_len, &class_method, &class_method_len) == FAILURE) {
        return;
    }
    zval *rule_item = linger_router_rule_instance(uri, class, class_method TSRMLS_CC);
    zval *rules = zend_read_property(router_ce, getThis(), ZEND_STRL(LINGER_ROUTER_PROPERTIES_RULES), 1 TSRMLS_CC);
    add_next_index_zval(rules, rule_item);
    zend_update_property(router_ce, getThis(), ZEND_STRL(LINGER_ROUTER_PROPERTIES_RULES), rules TSRMLS_CC);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(linger_framework_router, get)
{

}

PHP_METHOD(linger_framework_router, post)
{

}

PHP_METHOD(linger_framework_router, put)
{

}

PHP_METHOD(linger_framework_router, delete)
{

}

zend_function_entry router_methods[] = {
    PHP_ME(linger_framework_router, __construct, NULL, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_router, add, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router, get, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router, post, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router, put, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router, delete, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(router)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\Router", router_methods);
    router_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(router_ce, ZEND_STRL(LINGER_ROUTER_PROPERTIES_INSTANCE), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);
    zend_declare_property_null(router_ce, ZEND_STRL(LINGER_ROUTER_PROPERTIES_RULES), ZEND_ACC_PROTECTED);
    return SUCCESS;
}
