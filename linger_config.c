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

zend_class_entry *config_ce;

#define CONFIG_PROPERTIES_INSTANCE  "_instance"
#define CONFIG_PROPERTIES_CONFIG    "_config"

zval *linger_config_instance(zval *this, zval *config TSRMLS_DC)
{
    zval *instance = zend_read_static_property(config_ce, ZEND_STRL(CONFIG_PROPERTIES_INSTANCE), 1 TSRMLS_CC);
    if (Z_TYPE_P(instance) == IS_OBJECT &&
            !instanceof_function(Z_OBJCE_P(instance), config_ce)) {
        return instance;
    }
    if (this) {
        instance = this;
    } else {
        instance = NULL;
        MAKE_STD_ZVAL(instance);
        object_init_ex(instance, config_ce);
    }
    zend_update_property(config_ce, instance, ZEND_STRL(CONFIG_PROPERTIES_CONFIG), config TSRMLS_CC);
    zend_update_static_property(config_ce, ZEND_STRL(CONFIG_PROPERTIES_INSTANCE), instance TSRMLS_CC);
    return instance;
}

PHP_METHOD(linger_framework_config, __construct)
{
    zval *config;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &config) == FAILURE) {
        zval *prop;
        array_init(prop);
        zend_update_property(config_ce, getThis(), ZEND_STRL(CONFIG_PROPERTIES_CONFIG), prop TSRMLS_CC);
        zval_ptr_dtor(&prop);
        return;
    }
    (void)linger_config_instance(getThis(), config TSRMLS_CC);
}

PHP_METHOD(linger_framework_config, get)
{
    char *key;
    uint key_len = 0;
    zval **ppzval;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &key, &key_len) == FAILURE) {
        return;
    }
    if (!key_len) {
        RETURN_ZVAL(getThis(), 1, 0);
    } else {
        zval *config;
        HashTable *ht;
        long lval;
        double dval;
        config = zend_read_property(config_ce, getThis(), ZEND_STRL(CONFIG_PROPERTIES_CONFIG), 1 TSRMLS_CC);
        ht = Z_ARRVAL_P(config);
        if (is_numeric_string(key, key_len, &lval, &dval, 0) != IS_LONG) {
            if (zend_hash_find(ht, key, key_len + 1, (void **)&ppzval) == FAILURE) {
                RETURN_FALSE;
            }
        } else {
            if (zend_hash_index_find(ht, lval, (void **)&ppzval) == FAILURE) {
                RETURN_FALSE;
            }
        }
    }
    RETURN_ZVAL(*ppzval, 1, 0);
}

PHP_METHOD(linger_framework_config, set)
{
    zval *key;
    zval *value;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zz", &key, &value) == FAILURE) {
        RETURN_FALSE;
    }
    if (Z_TYPE_P(key) != IS_STRING) {
        RETURN_FALSE;
    }
    zval *config = zend_read_property(config_ce, getThis(), ZEND_STRL(CONFIG_PROPERTIES_CONFIG), 1 TSRMLS_CC);
    Z_ADDREF_P(value);
    if (zend_hash_update(Z_ARRVAL_P(config), Z_STRVAL_P(key), Z_STRLEN_P(key) + 1, (void **)&value, sizeof(zval *), NULL) == FAILURE) {
        Z_DELREF_P(value);
        RETURN_FALSE;
    } else {
        RETURN_TRUE;
    }
}

PHP_METHOD(linger_framework_config, __destruct)
{

}

zend_function_entry config_methods[] = {
    PHP_ME(linger_framework_config, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_config, get, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_config, set, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_config, __destruct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(config)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\Config", config_methods);
    config_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(config_ce, ZEND_STRL(CONFIG_PROPERTIES_INSTANCE), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC TSRMLS_CC);
    return SUCCESS;
}
