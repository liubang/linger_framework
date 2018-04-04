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
#include "ext/pcre/php_pcre.h"
#include "ext/standard/php_string.h"
#include "php_linger_framework.h"
#include "linger_router_rule.h"
#include "linger_request.h"

zend_class_entry *router_ce;

ZEND_BEGIN_ARG_INFO_EX(linger_framework_router_add_arginfo, 0, 0, 1)
ZEND_ARG_OBJ_INFO(0, rule_item, Linger\\Framework\\RouterRule, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(linger_framework_router_3_arginfo, 0, 0, 3)
ZEND_ARG_INFO(0, uri)
ZEND_ARG_INFO(0, class)
ZEND_ARG_INFO(0, method)
ZEND_END_ARG_INFO()

zval *linger_router_instance(zval *this)
{
    zval *instance = zend_read_static_property(router_ce, ZEND_STRL(LINGER_ROUTER_PROPERTIES_INSTANCE), 1, NULL);
    if (Z_TYPE_P(instance) == IS_OBJECT &&
            instanceof_function(Z_OBJCE_P(instance), router_ce TSRMLS_CC)) {
        return instance;
    }
    if (Z_ISUNDEF_P(this)) {
        object_init_ex(this, router_ce);
        zval router_rules = {{0}};
        array_init(&router_rules);
        zend_update_property(router_ce, this, ZEND_STRL(LINGER_ROUTER_PROPERTIES_RULES), router_rules);
        zval_ptr_dtor(&router_rules);
    }

    return this;
}

PHP_METHOD(linger_framework_router, __construct)
{

}

static void linger_router_add_rule(zval *this, zval *rule)
{
    if (EXPECTED(IS_OBJECT == Z_TYPE_P(rule_item)
                 && instanceof_function(Z_OBJCE_P(rule_item), router_rule_ce))) {
        zval *rules = zend_read_property(router_ce, this, ZEND_STRL(LINGER_ROUTER_PROPERTIES_RULES), 1, NULL);
        add_next_index_zval(rules, rule);
        Z_TRY_ADDREF_P(rule);
    } else {
        linger_throw_exception(NULL, 0, "parameter must be a instance of class %s.", router_rule_ce->name);
    }
}

PHP_METHOD(linger_framework_router, add)
{
    zval *rule_item;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &rule_item) == FAILURE) {
        return;
    }

    zval *self = getThis();
    linger_router_add_rule(self, rule_item);

    RETURN_ZVAL(self, 1, 0);
}

#define LINGER_GEN_METHOD(m) \
	PHP_METHOD(linger_framework_router, m) \
	{ \
		zval *uri = NULL,\
			*class = NULL, \
			*method = NULL; \
		if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zzz", &uri, &class, &method) == FAILURE) { \
			return; \
		} \
		zval req_method = {{0}}; \
		ZVAL_STRING(&req_method, ##m); \
		zval rule = {{0}}; \
		(void)linger_router_rule_instance(&rule, &req_method, uri, class, method); \
		zval_ptr_dtor(&req_method); \
		zval *self = getThis(); \
		linger_router_add_rule(self, &rule); \
		RETURN_ZVAL(self, 1, 0); \
	}

LINGER_GEN_METHOD(get);
LINGER_GEN_METHOD(put);
LINGER_GEN_METHOD(post);
LINGER_GEN_METHOD(delete);

zend_function_entry router_methods[] = {
    PHP_ME(linger_framework_router, __construct,, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_router, add, linger_framework_router_add_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router, get,, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router, put,, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router, post,, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router, delete,, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(router)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\Router", router_methods);
    router_ce = zend_register_internal_class_ex(&ce, NULL);
    zend_declare_property_null(router_ce, ZEND_STRL(LINGER_ROUTER_PROPERTIES_INSTANCE), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);
    zend_declare_property_null(router_ce, ZEND_STRL(LINGER_ROUTER_PROPERTIES_RULES), ZEND_ACC_PROTECTED);

    return SUCCESS;
}
