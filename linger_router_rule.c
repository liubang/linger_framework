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

zend_class_entry *router_rule_ce;

#define LINGER_ROUTER_RULE_PROPERTIES_REQUEST_METHOD "_request_method"
#define LINGER_ROUTER_RULE_PROPERTIES_URI            "_uri"
#define LINGER_ROUTER_RULE_PROPERTIES_CLASS          "_class"
#define LINGER_ROUTER_RULE_PROPERTIES_CLASS_METHOD   "_class_method"

zval *linger_router_rule_instance(char *uri, char *class, char *class_method TSRMLS_DC)
{

}

PHP_METHOD(linger_framework_router_rule, __construct)
{
    char *method, *uri,*class,*class_method;
    uint method_len, uri_len, class_len, class_method_len;
    if (zend_parse_parameters(ZEND_NUM_ARGS TSRMLS_CC, "ssss", 
                &method, &method_len, &uri, &uri_len, &class, &class_len, &class_method, &class_method_len) == FAILURE) {
        return;
    }
    
    
    
}

zend_function_entry router_rule_methods[] = {
    PHP_FE(linger_framework_router_rule, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(router_rule)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\RouterRule", router_rule_methods);
    router_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(router_rule_ce, ZEND_STRL(LINGER_ROUTER_RULE_PROPERTIES_REQUEST_METHOD), ZEND_ACC_PRIVATE);
    zend_declare_property_null(router_rule_ce, ZEND_STRL(LINGER_ROUTER_RULE_PROPERTIES_URI), ZEND_ACC_PRIVATE);
    zend_declare_property_null(router_rule_ce, ZEND_STRL(LINGER_ROUTER_RULE_PROPERTIES_CLASS), ZEND_ACC_PRIVATE);
    zend_declare_property_null(router_rule_ce, ZEND_STRL(LINGER_ROUTER_RULE_PROPERTIES_CLASS_METHOD), ZEND_ACC_PRIVATE);
    return SUCCESS;
}
