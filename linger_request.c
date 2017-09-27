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
#include "main/SAPI.h"
#include "php_linger_framework.h"

zend_class_entry *request_ce;

#define REQUEST_PROPERTIES_INSTANCE     "_instance"
#define REQUEST_PROPERTIES_METHOD       "_method"
#define REQUEST_PROPERTIES_HEADER       "_header"
#define REQUEST_PROPERTIES_COOKIE       "_cookie"
#define REQUEST_PROPERTIES_QUERY        "_query"
#define REQUEST_PROPERTIES_PARAM        "_param"
#define REQUEST_PROPERTIES_POST         "_post"
#define REQUEST_PROPERTIES_FILES        "_files"
#define REQUEST_PROPERTIES_RAWCONTENT   "_rawcontent"

zval *linger_request_instance(zval *this TSRMLS_DC) {
    zval *instance = zend_read_static_property(request_ce, ZEND_STRL(REQUEST_PROPERTIES_INSTANCE), 1 TSRMLS_CC);
    if (Z_TYPE_P(instance) == IS_OBJECT &&
            instanceof_function(Z_OBJCE_P(instance), request_ce)) {
        return instance;
    }
    if (this) {
        instance = this;
    } else {
        instance = NULL;
        MAKE_STD_ZVAL(instance);
        object_init_ex(instance, request_ce);
    }
    zend_update_static_property(request_ce, ZEND_STRL(REQUEST_PROPERTIES_INSTANCE), instance TSRMLS_CC);
    zval *method;
    MAKE_STD_ZVAL(method);
    if (SG(request_info).request_method) {
        ZVAL_STRING(method, (char *)SG(request_info).request_method, 1);
    } else if (strncasecmp(sapi_module.name, "cli", 3)) {
        ZVAL_STRING(method, "Unknow", 1);
    } else {
        ZVAL_STRING(method, "CLI", 1);
    }
    zend_update_property(request_ce, instance, ZEND_STRL(REQUEST_PROPERTIES_METHOD), method TSRMLS_CC);
    zval_ptr_dtor(&method);
    zend_update_property(request_ce, instance, ZEND_STRL(REQUEST_PROPERTIES_POST), PG(http_globals)[TRACK_VARS_POST] TSRMLS_CC);
    zend_update_property(request_ce, instance, ZEND_STRL(REQUEST_PROPERTIES_QUERY), PG(http_globals)[TRACK_VARS_GET] TSRMLS_CC);
    zend_update_property(request_ce, instance, ZEND_STRL(REQUEST_PROPERTIES_FILES), PG(http_globals)[TRACK_VARS_FILES] TSRMLS_CC);
    zend_update_property(request_ce, instance, ZEND_STRL(REQUEST_PROPERTIES_COOKIE), PG(http_globals)[TRACK_VARS_COOKIE] TSRMLS_CC);
    zend_update_property(request_ce, instance, ZEND_STRL(REQUEST_PROPERTIES_HEADER), PG(http_globals)[TRACK_VARS_SERVER] TSRMLS_CC);
    return instance;
}

PHP_METHOD(linger_framework_request, __construct)
{
    (void)linger_request_instance(getThis() TSRMLS_CC);
}

PHP_METHOD(linger_framework_request, getMethod)
{
    zval *method = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_METHOD), 1 TSRMLS_CC);
    RETURN_ZVAL(method, 1, 0);
}

PHP_METHOD(linger_framework_request, getQuery)
{
    char *key;
    uint key_len = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &key, &key_len) == FAILURE) {
        return;
    }

    zval *query = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_QUERY), 1 TSRMLS_CC);

    if (!key_len) {
        RETURN_ZVAL(query, 1, 0);
    } else {
        HashTable *ht = Z_ARRVAL_P(query);
        zval **ret;
        if (zend_hash_find(ht, key, key_len + 1, (void **)&ret) == FAILURE) {
            RETURN_NULL();
        } else {
            RETURN_ZVAL(*ret, 1, 0);
        }
    }
}

PHP_METHOD(linger_framework_request, getParam)
{
    char *key;
    uint key_len = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &key, &key_len) == FAILURE) {
        return;
    }
    zval *param = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_PARAM), 1 TSRMLS_CC);
    if (!key_len) {
        RETURN_ZVAL(param, 1, 0);
    } else {
        HashTable *ht = Z_ARRVAL_P(param);
        zval **ret;
        if (zend_hash_find(ht, key, key_len + 1, (void **)&ret) == FAILURE) {
            RETURN_NULL();
        } else {
            RETURN_ZVAL(*ret, 1, 0);
        }
    }
}

PHP_METHOD(linger_framework_request, getPost)
{
    char *key;
    uint key_len = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &key, &key_len) == FAILURE) {
        return;
    }
    zval *post = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_POST), 1 TSRMLS_CC);
    if (!key_len) {
        RETURN_ZVAL(post, 1, 0);
    } else {
        HashTable *ht = Z_ARRVAL_P(post);
        zval **ret;
        if (zend_hash_find(ht, key, key_len + 1, (void **)&ret) == FAILURE) {
            RETURN_NULL();
        } else {
            RETURN_ZVAL(*ret, 1, 0);
        }
    }
}

PHP_METHOD(linger_framework_request, getHeader)
{
    char *key;
    uint key_len = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &key, &key_len) == FAILURE) {
        return;
    }
    zval *header = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_HEADER), 1 TSRMLS_CC);
    if (!key_len) {
        RETURN_ZVAL(header, 1, 0);
    } else {
        HashTable *ht = Z_ARRVAL_P(header);
        zval **ret;
        if (zend_hash_find(ht, key, key_len + 1, (void **)&ret) == FAILURE) {
            RETURN_NULL();
        } else {
            RETURN_ZVAL(*ret, 1, 0);
        }
    }
}

PHP_METHOD(linger_framework_request, getCookie)
{

}

PHP_METHOD(linger_framework_request, rawcontent)
{

}

PHP_METHOD(linger_framework_request, isGet)
{

}

PHP_METHOD(linger_framework_request, isPost)
{

}

PHP_METHOD(linger_framework_request, isAjax)
{

}

zend_function_entry request_methods[] = {
    PHP_ME(linger_framework_request, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_request, getMethod, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, getQuery, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, getParam, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, getPost, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, getHeader, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, getCookie, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, rawcontent, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, isGet, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, isPost, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, isAjax, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(request)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\Request", request_methods);
    request_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_INSTANCE), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_METHOD), ZEND_ACC_PROTECTED);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_HEADER), ZEND_ACC_PROTECTED);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_COOKIE), ZEND_ACC_PROTECTED);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_QUERY), ZEND_ACC_PROTECTED);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_PARAM), ZEND_ACC_PROTECTED);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_POST), ZEND_ACC_PROTECTED);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_FILES), ZEND_ACC_PROTECTED);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_RAWCONTENT), ZEND_ACC_PROTECTED);
    return SUCCESS;
}
