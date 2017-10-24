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
#include "Zend/zend_interfaces.h"
#include "ext/standard/url.h"
#include "ext/standard/php_string.h"

zend_class_entry *request_ce;

#define REQUEST_PROPERTIES_INSTANCE     "_instance"
#define REQUEST_PROPERTIES_METHOD       "_method"
#define REQUEST_PROPERTIES_URI          "_uri"
#define REQUEST_PROPERTIES_COOKIE       "_cookie"
#define REQUEST_PROPERTIES_QUERY        "_query"
#define REQUEST_PROPERTIES_PARAM        "_param"
#define REQUEST_PROPERTIES_POST         "_post"
#define REQUEST_PROPERTIES_FILES        "_files"

zval *linger_request_instance(zval *this, zval *uri TSRMLS_DC)
{
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
#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 4)
    zend_bool jit_initialization = (PG(auto_globals_jit) && !PG(register_globals) && !PG(register_long_arrays));
#else
    zend_bool jit_initialization = PG(auto_globals_jit);
#endif
    if (jit_initialization) {
        zend_is_auto_global(ZEND_STRL("_SERVER") TSRMLS_CC);
    }
    zend_update_property(request_ce, instance, ZEND_STRL(REQUEST_PROPERTIES_POST), PG(http_globals)[TRACK_VARS_POST] TSRMLS_CC);
    zend_update_property(request_ce, instance, ZEND_STRL(REQUEST_PROPERTIES_QUERY), PG(http_globals)[TRACK_VARS_GET] TSRMLS_CC);
    zend_update_property(request_ce, instance, ZEND_STRL(REQUEST_PROPERTIES_FILES), PG(http_globals)[TRACK_VARS_FILES] TSRMLS_CC);
    zend_update_property(request_ce, instance, ZEND_STRL(REQUEST_PROPERTIES_COOKIE), PG(http_globals)[TRACK_VARS_COOKIE] TSRMLS_CC);
    zval *param;
    MAKE_STD_ZVAL(param);
    array_init(param);
    zend_update_property(request_ce, instance, ZEND_STRL(REQUEST_PROPERTIES_PARAM), param TSRMLS_CC);
    zval_ptr_dtor(&param);

    if (uri == NULL) {
        zval *server = PG(http_globals)[TRACK_VARS_SERVER];
        HashTable *ht = Z_ARRVAL_P(server);
        zval **ret;
        if (zend_hash_find(ht, ZEND_STRS("REQUEST_URI"), (void **)&ret) == SUCCESS) {
            if (strstr(Z_STRVAL_P(*ret), "http") == Z_STRVAL_P(*ret)) {
                php_url *url_info = php_url_parse(Z_STRVAL_P(*ret));
                if (url_info && url_info->path) {
                    zval *tmpuri;
                    MAKE_STD_ZVAL(tmpuri);
                    ZVAL_STRING(tmpuri, url_info->path, 1);
                    zend_update_property(request_ce, instance, ZEND_STRL(REQUEST_PROPERTIES_URI), tmpuri TSRMLS_CC);
                    zval_ptr_dtor(&tmpuri);
                }
            } else {
                char *pos = NULL;
                if ((pos = strstr(Z_STRVAL_P(*ret), "?"))) {
                    zval *tmpuri;
                    MAKE_STD_ZVAL(tmpuri);
                    ZVAL_STRINGL(tmpuri, Z_STRVAL_P(*ret), pos - Z_STRVAL_P(*ret), 1);
                    zend_update_property(request_ce, instance, ZEND_STRL(REQUEST_PROPERTIES_URI), tmpuri TSRMLS_CC);
                    zval_ptr_dtor(&tmpuri);
                } else {
                    zend_update_property(request_ce, instance, ZEND_STRL(REQUEST_PROPERTIES_URI), *ret TSRMLS_CC);
                }
            }
            zval_ptr_dtor(ret);
        }
    } else {
        zend_update_property(request_ce, instance, ZEND_STRL(REQUEST_PROPERTIES_URI), uri TSRMLS_CC);
    }
    return instance;
}

zval *linger_request_get_request_uri(zval *this TSRMLS_DC)
{
    if (this != NULL) {
        zval *uri = zend_read_property(request_ce, this, ZEND_STRL(REQUEST_PROPERTIES_URI), 1 TSRMLS_CC);
        if (Z_TYPE_P(uri) == IS_STRING) {
            return uri;
        }
    }
    return NULL;
}

zval *linger_request_get_request_method(zval *this TSRMLS_DC)
{
    if (this != NULL) {
        zval *request_method = zend_read_property(request_ce, this, ZEND_STRL(REQUEST_PROPERTIES_METHOD), 1 TSRMLS_CC);
        if (Z_TYPE_P(request_method) == IS_STRING) {
            return request_method;
        }
    }
    return NULL;
}

void *linger_request_set_param(zval *this, char *key, char *val TSRMLS_DC)
{
    zval *param = zend_read_property(request_ce, this, ZEND_STRL(REQUEST_PROPERTIES_PARAM), 1 TSRMLS_CC);
    add_assoc_string(param, key, val, 1);
    zend_update_property(request_ce, this, ZEND_STRL(REQUEST_PROPERTIES_PARAM), param TSRMLS_CC);
}

int linger_request_set_params(zval *this, zval *values TSRMLS_DC)
{
    if (values && IS_ARRAY == Z_TYPE_P(values)) {
        zval *params = zend_read_property(request_ce, this, ZEND_STRL(REQUEST_PROPERTIES_PARAM), 1 TSRMLS_CC);
        zend_hash_copy(Z_ARRVAL_P(params), Z_ARRVAL_P(values), (copy_ctor_func_t) zval_add_ref, NULL, sizeof(zval *));
        return SUCCESS;
    }
    return FAILURE;
}

PHP_METHOD(linger_framework_request, __construct)
{
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
    zval *default_value = NULL;
    zval *filter = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|szz", &key, &key_len, &default_value, &filter) == FAILURE) {
        RETURN_FALSE;
    }
    zval *query = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_QUERY), 1 TSRMLS_CC);
    if (!key_len) {
        RETURN_ZVAL(query, 1, 0);
    } else {
        HashTable *ht = Z_ARRVAL_P(query);
        zval **ret;
        if (zend_hash_find(ht, key, key_len + 1, (void **)&ret) == FAILURE) {
            if (default_value) {
                RETURN_ZVAL(default_value, 1, 0);
            }  else {
                RETURN_NULL();
            }
        } else {
            char *func_name = NULL;
            int func_name_len = 0;
            if (filter && zend_is_callable_ex(filter, NULL, 0, &func_name, &func_name_len, NULL, NULL TSRMLS_CC)) {
                zval *ret_val;
                zend_call_method(NULL, NULL, NULL, func_name, func_name_len, &ret_val, 1, *ret, NULL TSRMLS_CC);
                linger_efree(func_name);
                RETURN_ZVAL(ret_val, 1, 0);
            } else {
                linger_efree(func_name);
                RETURN_ZVAL(*ret, 1, 0);
            }
        }
    }
}

PHP_METHOD(linger_framework_request, getParam)
{
    char *key;
    uint key_len = 0;
    zval *default_value = NULL;
    zval *filter = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|szz", &key, &key_len, &default_value, &filter) == FAILURE) {
        RETURN_FALSE;
    }
    zval *param = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_PARAM), 1 TSRMLS_CC);
    if (!key_len) {
        RETURN_ZVAL(param, 1, 0);
    } else {
        HashTable *ht = Z_ARRVAL_P(param);
        zval **ret;
        if (zend_hash_find(ht, key, key_len + 1, (void **)&ret) == FAILURE) {
            if (default_value) {
                RETURN_ZVAL(default_value, 1, 0);
            } else {
                RETURN_NULL();
            }
        } else {
            char *func_name = NULL;
            int func_name_len = 0;
            if (filter && zend_is_callable_ex(filter, NULL, 0, &func_name, &func_name_len, NULL, NULL TSRMLS_CC)) {
                zval *ret_val;
                zend_call_method(NULL, NULL, NULL, func_name, func_name_len, &ret_val, 1, *ret, NULL TSRMLS_CC);
                linger_efree(func_name);
                RETURN_ZVAL(ret_val, 1, 0);
            } else {
                linger_efree(func_name);
                RETURN_ZVAL(*ret, 1, 0);
            }
        }
    }
}

PHP_METHOD(linger_framework_request, getPost)
{
    char *key;
    uint key_len = 0;
    zval *default_value = NULL;
    zval *filter = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|szz", &key, &key_len, &default_value, &filter) == FAILURE) {
        RETURN_FALSE;
    }
    zval *post = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_POST), 1 TSRMLS_CC);
    if (!key_len) {
        RETURN_ZVAL(post, 1, 0);
    } else {
        HashTable *ht = Z_ARRVAL_P(post);
        zval **ret;
        if (zend_hash_find(ht, key, key_len + 1, (void **)&ret) == FAILURE) {
            if (default_value) {
                RETURN_ZVAL(default_value, 1, 0);
            } else {
                RETURN_NULL();
            }
        } else {
            char *func_name = NULL;
            int func_name_len = 0;
            if (filter && zend_is_callable_ex(filter, NULL, 0, &func_name, &func_name_len, NULL, NULL TSRMLS_CC)) {
                zval *ret_val;
                zend_call_method(NULL, NULL, NULL, func_name, func_name_len, &ret_val, 1, *ret, NULL TSRMLS_CC);
                linger_efree(func_name);
                RETURN_ZVAL(ret_val, 1, 0);
            } else {
                linger_efree(func_name);
                RETURN_ZVAL(*ret, 1, 0);
            }
        }
    }
}


PHP_METHOD(linger_framework_request, getCookie)
{
    char *key;
    uint key_len = 0;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|s", &key, &key_len) == FAILURE) {
        return;
    }
    zval *cookie = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_COOKIE), 1 TSRMLS_CC);
    if (!key_len) {
        RETURN_ZVAL(cookie, 1, 0);
    } else {
        HashTable *ht = Z_ARRVAL_P(cookie);
        zval **ret;
        if (zend_hash_find(ht, key, key_len + 1, (void **)&ret) == FAILURE) {
            RETURN_NULL();
        } else {
            RETURN_ZVAL(*ret, 1, 0);
        }
    }
}

PHP_METHOD(linger_framework_request, isGet)
{
    zval *method = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_METHOD), 1 TSRMLS_CC);
    if (Z_TYPE_P(method) == IS_STRING) {
        if (!strncasecmp(Z_STRVAL_P(method), "GET", 3)) {
            RETURN_TRUE;
        }
    }
    RETURN_FALSE;
}

PHP_METHOD(linger_framework_request, isPost)
{
    zval *method = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_METHOD), 1 TSRMLS_CC);
    if (Z_TYPE_P(method) == IS_STRING) {
        if (!strncasecmp(Z_STRVAL_P(method), "POST", 4)) {
            RETURN_TRUE;
        }
    }
    RETURN_FALSE;
}

PHP_METHOD(linger_framework_request, isAjax)
{
#if (PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 4)
    zend_bool jit_initialization = (PG(auto_globals_jit) && !PG(register_globals) && !PG(register_long_arrays));
#else
    zend_bool jit_initialization = PG(auto_globals_jit);
#endif
    if (jit_initialization) {
        zend_is_auto_global(ZEND_STRL("_SERVER") TSRMLS_CC);
    }
    zval *server = PG(http_globals)[TRACK_VARS_SERVER];
    HashTable *arr = Z_ARRVAL_P(server);
    zval **ret;
    if (zend_hash_find(arr, ZEND_STRS("HTTP_X_REQUEST_WITH"), (void **)&ret) == SUCCESS) {
        if (Z_TYPE_P(*ret) == IS_STRING) {
            RETURN_TRUE;
        }
        zval_ptr_dtor(ret);
    }
    RETURN_FALSE;
}

PHP_METHOD(linger_framework_request, setMethod)
{
    zval *method;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &method) == FAILURE) {
        return;
    }
    if (Z_TYPE_P(method) == IS_STRING) {
        char *lower_method = zend_str_tolower_dup(Z_STRVAL_P(method), Z_STRLEN_P(method));
        if (!strncmp(lower_method, "get", 3)
                || !strncmp(lower_method, "post", 4)
                || !strncmp(lower_method, "put", 3)
                || !strncmp(lower_method, "delete", 6)) {
            zend_update_property_string(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_METHOD), lower_method TSRMLS_CC);
            linger_efree(lower_method);
        }
    }
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(linger_framework_request, setUri)
{
    zval *uri;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &uri) == FAILURE) {
        return;
    }
    if (Z_TYPE_P(uri) == IS_STRING) {
        zend_update_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_URI), uri TSRMLS_CC);
    } else {
        linger_throw_exception(NULL, 0, "uri must be string.");
        return;
    }
    RETURN_ZVAL(getThis(), 1, 0);
}

zend_function_entry request_methods[] = {
    PHP_ME(linger_framework_request, __construct, NULL, ZEND_ACC_PROTECTED | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_request, getMethod, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, getQuery, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, getParam, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, getPost, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, getCookie, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, isGet, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, isPost, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, isAjax, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, setUri, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, setMethod, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(request)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\Request", request_methods);
    request_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_INSTANCE), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_METHOD), ZEND_ACC_PROTECTED);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_URI), ZEND_ACC_PROTECTED);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_COOKIE), ZEND_ACC_PROTECTED);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_QUERY), ZEND_ACC_PROTECTED);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_PARAM), ZEND_ACC_PROTECTED);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_POST), ZEND_ACC_PROTECTED);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_FILES), ZEND_ACC_PROTECTED);
    return SUCCESS;
}
