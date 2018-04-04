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
#include "php_linger_framework.h"
#include "ext/standard/php_string.h"

zend_class_entry *router_rule_ce;

ZEND_BEGIN_ARG_INFO_EX(linger_framework_router_rule_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(linger_framework_router_rule_4_arginfo, 0, 0, 4)
ZEND_ARG_INFO(0, request_method)
ZEND_ARG_INFO(0, uri)
ZEND_ARG_INFO(0, class)
ZEND_ARG_INFO(0, class_method)
ZEND_END_ARG_INFO()

zval *linger_router_rule_instance(zval *this, zval *request_method,
                                  zval *uri, zval *class, zval *class_method)
{
    if (Z_ISUNDEF_P(this)) {
        object_init_ex(this, router_rule_ce);
    }

    char *trimed_uri = php_trim(Z_STRVAL_P(uri), Z_STRLEN_P(uri), "/", 1, NULL, 3);
    char *format_uri = NULL;
    int format_uri_len = spprintf(&format_uri, 0, "/%s/", trimed_uri);
    linger_efree(trimed_uri);
    zend_update_property(router_rule_ce, this, ZEND_STRL(ROUTER_RULE_PROPERTIES_REQUEST_METHOD), request_method);
    zend_update_property_string(router_rule_ce, this, ZEND_STRL(ROUTER_RULE_PROPERTIES_URI), format_uri);
    zend_update_property(router_rule_ce, this, ZEND_STRL(ROUTER_RULE_PROPERTIES_CLASS), class);
    zend_update_property(router_rule_ce, this, ZEND_STRL(ROUTER_RULE_PROPERTIES_CLASS_METHOD), class_method);
    linger_efree(format_uri);

    return this;
}

zval *linger_router_rule_get_request_method(zval *this)
{
    if (!this)
        return NULL;
    return zend_read_property(router_rule_ce, this, ZEND_STRL(ROUTER_RULE_PROPERTIES_REQUEST_METHOD), 1, NULL);
}

zval *linger_router_rule_get_uri(zval *this)
{
    if (!this)
        return NULL;
    return zend_read_property(router_rule_ce, this, ZEND_STRL(ROUTER_RULE_PROPERTIES_URI), 1, NULL);
}

zval *linger_router_rule_get_class(zval *this)
{
    if (!this)
        return NULL;
    return zend_read_property(router_rule_ce, this, ZEND_STRL(ROUTER_RULE_PROPERTIES_CLASS), 1, NULL);
}

zval *linger_router_rule_get_class_method(zval *this)
{
    if (!this)
        return NULL;
    return zend_read_property(router_rule_ce, this, ZEND_STRL(ROUTER_RULE_PROPERTIES_CLASS_METHOD), 1, NULL);
}

PHP_METHOD(linger_framework_router_rule, __construct)
{
    zval *request_method, *uri, *class, *class_method;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zzzz",
                                    &request_method, &uri, &class, &class_method) == FAILURE) {
        return;
    }

    (void)linger_router_rule_instance(getThis(), request_method, uri, class, class_method);
}

PHP_METHOD(linger_framework_router_rule, getRequestMethod)
{
    zval *request_method = zend_read_property(router_rule_ce, getThis(), ZEND_STRL(ROUTER_RULE_PROPERTIES_REQUEST_METHOD), 1, NULL);

    RETURN_ZVAL(request_method, 1, 0);
}

PHP_METHOD(linger_framework_router_rule, getUri)
{
    zval *uri = zend_read_property(router_rule_ce, getThis(), ZEND_STRL(ROUTER_RULE_PROPERTIES_URI), 1, NULL);

    RETURN_ZVAL(uri, 1, 0);
}

PHP_METHOD(linger_framework_router_rule, getClass)
{
    zval *class = zend_read_property(router_rule_ce, getThis(), ZEND_STRL(ROUTER_RULE_PROPERTIES_CLASS), 1, NULL);

    RETURN_ZVAL(class, 1, 0);
}

PHP_METHOD(linger_framework_router_rule, getClassMethod)
{
    zval *class_method = zend_read_property(router_rule_ce, getThis(), ZEND_STRL(ROUTER_RULE_PROPERTIES_CLASS_METHOD), 1, NULL);

    RETURN_ZVAL(class_method, 1, 0);
}

zend_function_entry router_rule_methods[] = {
    PHP_ME(linger_framework_router_rule, __construct, linger_framework_router_rule_4_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_router_rule, getRequestMethod, linger_framework_router_rule_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router_rule, getUri, linger_framework_router_rule_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router_rule, getClass, linger_framework_router_rule_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router_rule, getClassMethod, linger_framework_router_rule_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(router_rule)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\RouterRule", router_rule_methods);
    router_rule_ce = zend_register_internal_class_ex(&ce, NULL);
    zend_declare_property_null(router_rule_ce, ZEND_STRL(ROUTER_RULE_PROPERTIES_REQUEST_METHOD), ZEND_ACC_PRIVATE);
    zend_declare_property_null(router_rule_ce, ZEND_STRL(ROUTER_RULE_PROPERTIES_URI), ZEND_ACC_PRIVATE);
    zend_declare_property_null(router_rule_ce, ZEND_STRL(ROUTER_RULE_PROPERTIES_CLASS), ZEND_ACC_PRIVATE);
    zend_declare_property_null(router_rule_ce, ZEND_STRL(ROUTER_RULE_PROPERTIES_CLASS_METHOD), ZEND_ACC_PRIVATE);
    return SUCCESS;
}

