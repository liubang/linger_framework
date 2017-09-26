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

zend_class_entry *config_ce;

#define CONFIG_PROPERTIES_INSTANCE  "_instance"
#define CONFIG_PROPERTIES_CONFIG    "_config"

zval *linger_config_instance(zval *oconfig, zval *config) {
    zval *instance = zend_read_static_property(config_ce, ZEND_STRL(CONFIG_PROPERTIES_INSTANCE), 1); 
    if (Z_TYPE_P(instance) != IS_OBJECT ||
            !instanceof_function(Z_OBJECT_P(instance), config_ce)) {
        object_init_ex(oconfig, config_ce);
        zend_update_property(config_ce, oconfig, ZEND_STRL(CONFIG_PROPERTIES_CONFIG), config);
        zend_update_static_property(config_ce, ZEND_STRL(CONFIG_PROPERTIES_INSTANCE), *oconfig);
    }
    return instance;
}

PHP_METHOD(linger_framework_config, __construct)
{
     
}

PHP_METHOD(linger_framework_config, get)
{

}

PHP_METHOD(linger_framework_config, set)
{

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
    return SUCCESS;
}
