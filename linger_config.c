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
#include "Zend/zend_interfaces.h"
#include "php_linger_framework.h"
#include "linger_config.h"

#if PHP_MAJOR_VERSION > 7
#include "linger_config_arginfo.h"
#else
#include "linger_config_legacy_arginfo.h"
#endif

#if defined(HAVE_SPL) && PHP_VERSION_ID < 70200
extern PHPAPI zend_class_entry *spl_ce_Countable;
#endif

zend_class_entry *config_ce;

zval *linger_config_instance(zval *this, zval *config)
{
    if (UNEXPECTED(!config || Z_TYPE_P(config) != IS_ARRAY)) {
        linger_throw_exception(NULL, 0, "config must be an array.");
        return NULL;
    }

    zval *instance = zend_read_static_property(config_ce, ZEND_STRL(CONFIG_PROPERTIES_INSTANCE), 1);

    if (Z_TYPE_P(instance) == IS_OBJECT &&
            !instanceof_function(Z_OBJCE_P(instance), config_ce)) {
        return instance;
    }

    if (Z_ISUNDEF_P(this)) {
        object_init_ex(this, config_ce);
    } else {
        return this;
    }

    zend_update_property(config_ce, Z_OBJ_P(this), ZEND_STRL(CONFIG_PROPERTIES_CONFIG), config);
    zend_update_static_property(config_ce, ZEND_STRL(CONFIG_PROPERTIES_INSTANCE), this);

    return this;
}

PHP_METHOD(linger_framework_config, __construct)
{
    zval *config;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &config) == FAILURE) {
        return;
    }

    (void)linger_config_instance(getThis(), config);
}

PHP_METHOD(linger_framework_config, get)
{
    zend_string *key = NULL;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|S", &key) == FAILURE) {
        return;
    }

    zval *config;
    config = zend_read_property(config_ce, Z_OBJ_P(getThis()), ZEND_STRL(CONFIG_PROPERTIES_CONFIG), 1, NULL);

    if (!key) {
        RETURN_ZVAL(config, 1, 0);
    } else {
        zval *ret;

        if ((ret = zend_hash_find(Z_ARRVAL_P(config), key)) == NULL) {
            RETURN_NULL();
        }

        RETURN_ZVAL(ret, 1, 0);
    }
}

PHP_METHOD(linger_framework_config, set)
{
    zend_string *key;
    zval *val;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "Sz", &key, &val) == FAILURE) {
        return;
    } else {
        zval *config = zend_read_static_property(config_ce, ZEND_STRL(CONFIG_PROPERTIES_CONFIG), 1);
        if (zend_hash_update(Z_ARRVAL_P(config), key, val) == NULL) {
            RETURN_FALSE;
        }
        Z_TRY_ADDREF_P(val);
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(linger_framework_config, has)
{
    zend_string *key;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "S", &key) == FAILURE) {
        return ;
    } else {
        zval *config = zend_read_property(config_ce, Z_OBJ_P(getThis()), ZEND_STRL(CONFIG_PROPERTIES_CONFIG), 1, NULL);
        RETURN_BOOL(zend_hash_exists(Z_ARRVAL_P(config), key));
    }
}

PHP_METHOD(linger_framework_config, del)
{
    zend_string *key;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "S", &key) == FAILURE) {
        return;
    } else {
        zval *config = zend_read_property(config_ce, Z_OBJ_P(getThis()), ZEND_STRL(CONFIG_PROPERTIES_CONFIG), 1, NULL);
        if (zend_hash_del(Z_ARRVAL_P(config), key) == SUCCESS) {
            RETURN_ZVAL(getThis(), 1, 0);
        }
    }

    RETURN_FALSE;
}

PHP_METHOD(linger_framework_config, clear)
{
    zval *config = zend_read_property(config_ce, Z_OBJ_P(getThis()), ZEND_STRL(CONFIG_PROPERTIES_CONFIG), 1, NULL);
    zend_hash_clean(Z_ARRVAL_P(config));
}

PHP_METHOD(linger_framework_config, count)
{
    zval *config = zend_read_property(config_ce, Z_OBJ_P(getThis()), ZEND_STRL(CONFIG_PROPERTIES_CONFIG), 1, NULL);
    RETURN_LONG(zend_hash_num_elements(Z_ARRVAL_P(config)));
}

PHP_METHOD(linger_framework_config, rewind)
{
    zval *config = zend_read_property(config_ce, Z_OBJ_P(getThis()), ZEND_STRL(CONFIG_PROPERTIES_CONFIG), 1, NULL);
    zend_hash_internal_pointer_reset(Z_ARRVAL_P(config));
}

PHP_METHOD(linger_framework_config, current)
{
    zval *config = zend_read_property(config_ce, Z_OBJ_P(getThis()), ZEND_STRL(CONFIG_PROPERTIES_CONFIG), 1, NULL);
    zval *pzval;
    if ((pzval = zend_hash_get_current_data(Z_ARRVAL_P(config))) == NULL) {
        RETURN_FALSE;
    }

    RETURN_ZVAL(pzval, 1, 0);
}

PHP_METHOD(linger_framework_config, key)
{
    zval *config = zend_read_property(config_ce, Z_OBJ_P(getThis()), ZEND_STRL(CONFIG_PROPERTIES_CONFIG), 1, NULL);
    zend_string *key;
    ulong index;
    if (zend_hash_get_current_key(Z_ARRVAL_P(config), &key, &index) == HASH_KEY_IS_LONG) {
        RETURN_LONG(index);
    } else {
        RETURN_STR(zend_string_copy(key));
    }
}

PHP_METHOD(linger_framework_config, next)
{
    zval *config = zend_read_property(config_ce, Z_OBJ_P(getThis()), ZEND_STRL(CONFIG_PROPERTIES_CONFIG), 1, NULL);
    zend_hash_move_forward(Z_ARRVAL_P(config));
}

PHP_METHOD(linger_framework_config, valid)
{
    zval *config = zend_read_property(config_ce, Z_OBJ_P(getThis()), ZEND_STRL(CONFIG_PROPERTIES_CONFIG), 1, NULL);
    RETURN_BOOL(zend_hash_has_more_elements(Z_ARRVAL_P(config)) == SUCCESS);
}

zend_function_entry config_methods[] = {
    PHP_ME(linger_framework_config, __construct,arginfo_class_Linger_Framework_Config___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_config, get, arginfo_class_Linger_Framework_Config_get,ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_config, set, arginfo_class_Linger_Framework_Config_set, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_config, has, arginfo_class_Linger_Framework_Config_has, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_config, del, arginfo_class_Linger_Framework_Config_del, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_config, count, arginfo_class_Linger_Framework_Config_count, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_config, rewind, arginfo_class_Linger_Framework_Config_rewind, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_config, next, arginfo_class_Linger_Framework_Config_next, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_config, current, arginfo_class_Linger_Framework_Config_current, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_config, key, arginfo_class_Linger_Framework_Config_key, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_config, valid, arginfo_class_Linger_Framework_Config_valid, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_config, clear, arginfo_class_Linger_Framework_Config_clear, ZEND_ACC_PUBLIC)
    PHP_MALIAS(linger_framework_config, offsetGet, get, arginfo_class_Linger_Framework_Config_offsetGet, ZEND_ACC_PUBLIC)
    PHP_MALIAS(linger_framework_config, offsetSet, set, arginfo_class_Linger_Framework_Config_offsetSet, ZEND_ACC_PUBLIC)
    PHP_MALIAS(linger_framework_config, offsetExists, has, arginfo_class_Linger_Framework_Config_offsetExists, ZEND_ACC_PUBLIC)
    PHP_MALIAS(linger_framework_config, offsetUnset, del, arginfo_class_Linger_Framework_Config_offsetUnSet, ZEND_ACC_PUBLIC)
    PHP_MALIAS(linger_framework_config, __get, get, arginfo_class_Linger_Framework_Config___get, ZEND_ACC_PUBLIC)
    PHP_MALIAS(linger_framework_config, __set, set, arginfo_class_Linger_Framework_Config___set, ZEND_ACC_PUBLIC)
    PHP_MALIAS(linger_framework_config, __isset, has, arginfo_class_Linger_Framework_Config___isset, ZEND_ACC_PUBLIC)
    PHP_MALIAS(linger_framework_config, __unset, del, arginfo_class_Linger_Framework_Config___unset, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(config)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\Config", config_methods);
    config_ce = zend_register_internal_class_ex(&ce, NULL);
    config_ce->ce_flags |= ZEND_ACC_FINAL;
#if defined(HAVE_SPL) && PHP_VERSION_ID < 70200
    zend_class_implements(config_ce, 3, zend_ce_iterator, zend_ce_arrayaccess, spl_ce_Countable);
#elif PHP_VERSION_ID >= 70200
    zend_class_implements(config_ce, 3, zend_ce_iterator, zend_ce_arrayaccess, zend_ce_countable);
#else
    zend_class_implements(config_ce, 2, zend_ce_iterator, zend_ce_arrayaccess);
#endif

    zend_declare_property_null(config_ce, ZEND_STRL(CONFIG_PROPERTIES_INSTANCE), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);
    zend_declare_property_null(config_ce, ZEND_STRL(CONFIG_PROPERTIES_CONFIG), ZEND_ACC_PROTECTED);

    return SUCCESS;
}
