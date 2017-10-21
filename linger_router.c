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
zend_class_entry *router_rule_ce;

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
    zval *router_rules;
    MAKE_STD_ZVAL(router_rules);
    array_init(router_rules);
    zend_update_property(router_ce, instance, ZEND_STRL(LINGER_ROUTER_PROPERTIES_RULES), router_rules TSRMLS_CC);
    zend_update_static_property(router_ce, ZEND_STRL(LINGER_ROUTER_PROPERTIES_INSTANCE), instance TSRMLS_CC);
    zval_ptr_dtor(&router_rules);
    return instance;
}

zval *linger_router_match(zval *dispatcher_obj, zval *this, char *uri TSRMLS_DC)
{

}

void linger_router_add_rule(zval *this, zval *rule TSRMLS_DC)
{
    zval *rules = zend_read_property(router_ce, this, ZEND_STRL(LINGER_ROUTER_PROPERTIES_RULES), 1 TSRMLS_CC);
    /* add refcount, that's important! */
    Z_ADDREF_P(rule);
    add_next_index_zval(rules, rule);
    zend_update_property(router_ce, this, ZEND_STRL(LINGER_ROUTER_PROPERTIES_RULES), rules TSRMLS_CC);
}

PHP_METHOD(linger_framework_router, __construct)
{
}

PHP_METHOD(linger_framework_router, add)
{
    zval *rule_item;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &rule_item) == FAILURE) {
        return;
    }
    if (IS_OBJECT == Z_TYPE_P(rule_item) &&
            instanceof_function(Z_OBJCE_P(rule_item), router_rule_ce)) {
        linger_router_add_rule(getThis(), rule_item TSRMLS_CC);
        RETURN_ZVAL(getThis(), 1, 0);
    } else {
        zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "parameter must be instanceof class %s", router_rule_ce->name);
        RETURN_FALSE;
    }
}

PHP_METHOD(linger_framework_router, get)
{
    zval *uri, *class, *class_method;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &uri, &class, &class_method) == FAILURE) {
        return;
    }
    zval *request_method;
    MAKE_STD_ZVAL(request_method);
    ZVAL_STRING(request_method,"get", 0);
    zval *rule = linger_router_rule_instance(NULL, request_method, uri, class, class_method);
    linger_router_add_rule(getThis(), rule TSRMLS_CC);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(linger_framework_router, post)
{
    zval *uri, *class, *class_method;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &uri, &class, &class_method) == FAILURE) {
        return;
    }
    zval *request_method;
    MAKE_STD_ZVAL(request_method);
    ZVAL_STRING(request_method, "post", 0);
    zval *rule = linger_router_rule_instance(NULL, request_method, uri, class, class_method);
    linger_router_add_rule(getThis(), rule TSRMLS_CC);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(linger_framework_router, put)
{
    zval *uri, *class, *class_method;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &uri, &class, &class_method) == FAILURE) {
        return;
    }
    zval *request_method;
    MAKE_STD_ZVAL(request_method);
    ZVAL_STRING(request_method, "put", 0);
    zval *rule = linger_router_rule_instance(NULL, request_method, uri, class, class_method);
    linger_router_add_rule(getThis(), rule TSRMLS_CC);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(linger_framework_router, delete)
{
    zval *uri, *class, *class_method;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &uri, &class, &class_method) == FAILURE) {
        return;
    }
    zval *request_method;
    MAKE_STD_ZVAL(request_method);
    ZVAL_STRING(request_method, "delete", 0);
    zval *rule = linger_router_rule_instance(NULL, request_method, uri, class, class_method);
    linger_router_add_rule(getThis(), rule TSRMLS_CC);
    RETURN_ZVAL(getThis(), 1, 0);
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
