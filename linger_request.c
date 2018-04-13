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
#include "SAPI.h"
#include "php_linger_framework.h"
#include "Zend/zend_interfaces.h"
#include "ext/standard/url.h"
#include "ext/standard/php_string.h"
#include "linger_request.h"

zend_class_entry *request_ce;

ZEND_BEGIN_ARG_INFO_EX(linger_framework_request_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(linger_framework_request_get_default_filter_arginfo, 0, 0, 3)
ZEND_ARG_TYPE_INFO(0, key, IS_STRING, 1)
ZEND_ARG_TYPE_INFO(0, default_value, IS_STRING, 1)
ZEND_ARG_CALLABLE_INFO(0, filter, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(linger_framework_request_get_arginfo, 0, 0, 1)
ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(linger_framework_request_set_method_arginfo, 0, 0, 1)
ZEND_ARG_INFO(0, method)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(linger_framework_request_set_uri_arginfo, 0, 0, 1)
ZEND_ARG_INFO(0, uri)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(linger_framework_request_set_param_arginfo, 0, 0, 1)
ZEND_ARG_INFO(0, param)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(linger_framework_request_set_post_arginfo, 0, 0, 1)
ZEND_ARG_INFO(0, post)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(linger_framework_request_set_query_arginfo, 0, 0, 1)
ZEND_ARG_INFO(0, query)
ZEND_END_ARG_INFO()

zval *linger_request_instance(zval *this)
{
    zval *instance = zend_read_static_property(request_ce, ZEND_STRL(REQUEST_PROPERTIES_INSTANCE), 1);
    if (instance && IS_OBJECT == Z_TYPE_P(instance) &&
            instanceof_function(Z_OBJCE_P(instance), request_ce)) {
        return instance;
    }
    if (Z_ISUNDEF_P(this)) {
        object_init_ex(this, request_ce);
    }
    zend_update_static_property(request_ce, ZEND_STRL(REQUEST_PROPERTIES_INSTANCE), this);
    zval method = {{0}},
    params = {{0}};
    if (SG(request_info).request_method) {
        ZVAL_STRING(&method, (char *)SG(request_info).request_method);
    } else if (strncasecmp(sapi_module.name, "cli", 3)) {
        ZVAL_STRING(&method, "Unknow");
    } else {
        ZVAL_STRING(&method, "Cli");
    }
    zend_update_property(request_ce, this, ZEND_STRL(REQUEST_PROPERTIES_METHOD), &method);
    zval_ptr_dtor(&method);

    array_init(&params);
    zend_update_property(request_ce, this, ZEND_STRL(REQUEST_PROPERTIES_PARAM), &params);
    zval_ptr_dtor(&params);

    zend_update_property(request_ce, this, ZEND_STRL(REQUEST_PROPERTIES_POST), &PG(http_globals)[TRACK_VARS_POST]);
    zend_update_property(request_ce, this, ZEND_STRL(REQUEST_PROPERTIES_QUERY), &PG(http_globals)[TRACK_VARS_GET]);
    zend_update_property(request_ce, this, ZEND_STRL(REQUEST_PROPERTIES_FILES), &PG(http_globals)[TRACK_VARS_FILES]);
    zend_update_property(request_ce, this, ZEND_STRL(REQUEST_PROPERTIES_COOKIE), &PG(http_globals)[TRACK_VARS_COOKIE]);

    zend_bool jit_init = PG(auto_globals_jit);
    if (jit_init) {
        zend_string *server_str = zend_string_init("_SERVER", sizeof("_SERVER") - 1, 0);
        zend_is_auto_global(server_str);
        zend_string_release(server_str);
    }
    zval *server = &PG(http_globals)[TRACK_VARS_SERVER];
    zend_array *ht = Z_ARRVAL_P(server);


#define REQ_GET_URI(ht, str, str_len)                       \
		i++;                                                \
		uri = zend_hash_str_find(ht, (char *)str, str_len); \
		if (uri)                                            \
			if (IS_STRING == Z_TYPE_P(uri))                 \
				goto handle;

    int i = 0;
    zval *uri;

    REQ_GET_URI(ht, "PATH_INFO", sizeof("PATH_INFO") - 1)
    REQ_GET_URI(ht, "REQUEST_URI", sizeof("REQUEST_URI") - 1)
    REQ_GET_URI(ht, "ORIG_PATH_INFO", sizeof("ORIG_PATH_INFO") - 1)

handle:
    if (uri) {
        if (i == 2) {
            if (!strncasecmp(Z_STRVAL_P(uri), "http", sizeof("http") - 1)) {
                php_url *url_info = php_url_parse(Z_STRVAL_P(uri));
                if (url_info && url_info->path) {
                    ZVAL_STRINGL(uri, url_info->path, strlen(url_info->path));
                }
                php_url_free(url_info);
            } else {
                char *pos = NULL;
                if ((pos = strstr(Z_STRVAL_P(uri), "?"))) {
                    ZVAL_STRINGL(uri, Z_STRVAL_P(uri), pos - Z_STRVAL_P(uri));
                }
            }
        }
        zend_update_property(request_ce, this, ZEND_STRL(REQUEST_PROPERTIES_URI), uri);
    }
    return this;
}

zval *linger_request_get_request_uri(zval *this)
{
    return zend_read_property(request_ce, this, ZEND_STRL(REQUEST_PROPERTIES_URI), 1, NULL);
}

zval *linger_request_get_request_method(zval *this)
{
    return zend_read_property(request_ce, this, ZEND_STRL(REQUEST_PROPERTIES_METHOD), 1, NULL);
}

int linger_request_set_param(zval *this, const char *key, char *val)
{
    zval *params = zend_read_property(request_ce, this, ZEND_STRL(REQUEST_PROPERTIES_PARAM), 1, NULL);
    return add_assoc_string(params, key, val);
}

int linger_request_set_params(zval *this, zval *values)
{
    zval *params = zend_read_property(request_ce, this, ZEND_STRL(REQUEST_PROPERTIES_PARAM), 1, NULL);
    if (values && IS_ARRAY == Z_TYPE_P(values)) {
        zend_hash_copy(Z_ARRVAL_P(params), Z_ARRVAL_P(values), (copy_ctor_func_t) zval_add_ref);
        return SUCCESS;
    } else {
        return FAILURE;
    }
}

int linger_request_set_post(zval *this, zval *values)
{
    zval *post= zend_read_property(request_ce, this, ZEND_STRL(REQUEST_PROPERTIES_POST), 1, NULL);
    if (values && IS_ARRAY == Z_TYPE_P(values)) {
        zend_hash_copy(Z_ARRVAL_P(post), Z_ARRVAL_P(values), (copy_ctor_func_t) zval_add_ref);
        return SUCCESS;
    } else {
        return FAILURE;
    }
}

PHP_METHOD(linger_framework_request, __construct)
{

}

PHP_METHOD(linger_framework_request, getMethod)
{
    zval *method = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_METHOD), 1, NULL);
    RETURN_ZVAL(method, 1, 0);
}

PHP_METHOD(linger_framework_request, getUri)
{
    zval *uri = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_URI), 1, NULL);
    RETURN_ZVAL(uri, 1, 0);
}

#define GET_D_F(htzv_ptr, strkey_ptr, zvd_ptr, zvfilter_ptr) \
	do { \
		if (!strkey_ptr) {\
			RETURN_ZVAL(htzv_ptr, 1, 0); \
		} else { \
			zval *ret;	 \
			if ((ret = zend_hash_find(Z_ARRVAL_P(htzv_ptr), strkey_ptr)) == NULL) { \
				if (zvd_ptr) { \
					RETURN_ZVAL(zvd_ptr, 1, 0); \
				} else { \
					RETURN_NULL(); \
				} \
			} else { \
				zend_string *callable_name; \
				zval retval = {{0}}; \
				if (zvfilter_ptr && zend_is_callable(zvfilter_ptr, 0, &callable_name)) { \
					zend_call_method(NULL, NULL, NULL, ZSTR_VAL(callable_name), ZSTR_LEN(callable_name), &retval, 1, ret, NULL); \
					zend_string_release(callable_name); \
					RETURN_ZVAL(&retval, 1, 0); \
				} else { \
					zend_string_release(callable_name); \
					RETURN_ZVAL(ret, 1, 0); \
				} \
			} \
		} \
	} while (0)

PHP_METHOD(linger_framework_request, getQuery)
{
    zend_string *key = NULL;
    zval *default_value = NULL, *filter = NULL;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|Szz", &key, &default_value, &filter) == FAILURE) {
        return;
    }

    zval *query = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_QUERY), 1, NULL);

    GET_D_F(query, key, default_value, filter);

    /*
    if (!key) {
        RETURN_ZVAL(query, 1, 0);
    } else {
        zval *ret;
        if ((ret = zend_hash_find(Z_ARRVAL_P(query), key)) == NULL) {
            if (default_value) {
                RETURN_ZVAL(default_value, 1, 0);
            } else {
                RETURN_NULL();
            }
        } else {
            // ZEND_API zend_bool zend_is_callable(zval *callable, uint32_t check_flags, zend_string **callable_name);
            zend_string *callable_name;
            zval retval = {{0}};
            if (filter && zend_is_callable(filter, 0, &callable_name)) {
                zend_call_method(NULL, NULL, NULL, ZSTR_VAL(callable_name), ZSTR_LEN(callable_name), &retval, 1, ret, NULL);
                zend_string_release(callable_name);
                RETURN_ZVAL(&retval, 1, 0);
            } else {
                zend_string_release(callable_name);
                RETURN_ZVAL(ret, 1, 0);
            }
        }
    }
    */
}

PHP_METHOD(linger_framework_request, getParam)
{
    zend_string *key = NULL;
    zval *default_value = NULL, *filter = NULL;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|Szz", &key, &default_value, &filter) == FAILURE) {
        return;
    }

    zval *params = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_PARAM), 1, NULL);

    GET_D_F(params, key, default_value, filter);
}

PHP_METHOD(linger_framework_request, getPost)
{
    zend_string *key = NULL;
    zval *default_value = NULL, *filter = NULL;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|Szz", &key, &default_value, &filter) == FAILURE) {
        return;
    }

    zval *post = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_POST), 1, NULL);

    GET_D_F(post, key, default_value, filter);
}

PHP_METHOD(linger_framework_request, getCookie)
{
    zend_string *key = NULL;
    zval *cookie = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_COOKIE), 1, NULL);

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|S", &key) == FAILURE) {
        return;
    }

    if (!key) {
        RETURN_ZVAL(cookie, 1, 0);
    } else {
        zval *ret;
        if ((ret = zend_hash_find(Z_ARRVAL_P(cookie), key)) == NULL) {
            RETURN_NULL();
        } else {
            RETURN_ZVAL(ret, 1, 0);
        }
    }
}

PHP_METHOD(linger_framework_request, getFile)
{
    zend_string *key = NULL;
    zval *files = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_FILES), 1, NULL);

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|S", &key) == FAILURE) {
        return;
    }

    if (!key) {
        RETURN_ZVAL(files, 1, 0);
    } else {
        zval *ret;
        if ((ret = zend_hash_find(Z_ARRVAL_P(files), key)) == NULL) {
            RETURN_NULL();
        } else {
            RETURN_ZVAL(ret, 1, 0);
        }
    }
}

PHP_METHOD(linger_framework_request, isAjax)
{
    zend_bool jit_init = PG(auto_globals_jit);
    if (jit_init) {
        zend_string *server_str = zend_string_init("_SERVER", sizeof("_SERVER") - 1, 0);
        zend_is_auto_global(server_str);
        zend_string_release(server_str);
    }
    zval *server = &PG(http_globals)[TRACK_VARS_SERVER];
    if (zend_hash_str_find(Z_ARRVAL_P(server), "HTTP_X_REQUEST_WITH", sizeof("HTTP_X_REQUEST_WITH") - 1) == NULL) {
        RETURN_FALSE;
    } else {
        RETURN_TRUE;
    }
}

PHP_METHOD(linger_framework_request, isPost)
{
    zval *method = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_METHOD), 1, NULL);

    if (EXPECTED(Z_TYPE_P(method) == IS_STRING)) {
        if (!strncasecmp(Z_STRVAL_P(method), "POST", 4)) {
            RETURN_TRUE;
        }
    }

    RETURN_FALSE;
}

PHP_METHOD(linger_framework_request, isGet)
{
    zval *method = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_METHOD), 1, NULL);

    if (EXPECTED(Z_TYPE_P(method) == IS_STRING)) {
        if (!strncasecmp(Z_STRVAL_P(method), "GET", 3)) {
            RETURN_TRUE;
        }
    }

    RETURN_FALSE;
}

PHP_METHOD(linger_framework_request, isCli)
{
    zval *method = zend_read_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_METHOD), 1, NULL);

    if (EXPECTED(IS_STRING == Z_TYPE_P(method))) {
        if (!strncasecmp(Z_STRVAL_P(method), "CLI", 3)) {
            RETURN_TRUE;
        }
    }

    RETURN_FALSE;
}

PHP_METHOD(linger_framework_request, setMethod)
{
    zval *method;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS() TSRMLS_CC, "z", &method) == FAILURE) {
        return;
    }

    if (Z_TYPE_P(method) == IS_STRING) {
        char *lower_method = zend_str_tolower_dup(Z_STRVAL_P(method), Z_STRLEN_P(method));
        if (!strncmp(lower_method, "get", 3)
                || !strncmp(lower_method, "post", 4)
                || !strncmp(lower_method, "put", 3)
                || !strncmp(lower_method, "delete", 6)) {
            zend_update_property_string(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_METHOD), lower_method TSRMLS_CC);
        }
        linger_efree(lower_method);
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(linger_framework_request, setUri)
{
    zval *uri;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &uri) == FAILURE) {
        return;
    }

    if (uri && IS_STRING == Z_TYPE_P(uri)) {
        zend_string *trimed_uri = php_trim(Z_STR(*uri), "/", 1, 3);
        char *format_uri = NULL;
        int format_uri_len = spprintf(&format_uri, 0, "/%s", ZSTR_VAL(trimed_uri));
        zend_string_release(trimed_uri);
        zend_update_property_string(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_URI), format_uri);
        linger_efree(format_uri);
    } else {
        linger_throw_exception(NULL, 0, "parameter must be string.");
        return;
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(linger_framework_request, setParam)
{
    zval *param;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &param) == FAILURE) {
        return;
    }

    if (Z_TYPE_P(param) == IS_ARRAY) {
        zend_update_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_PARAM), param);
    } else {
        linger_throw_exception(NULL, 0, "parameter must be an array.");
        return;
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(linger_framework_request, setPost)
{
    zval *post;
    if (zend_parse_parameters(ZEND_NUM_ARGS(), "z", &post) == FAILURE) {
        return;
    }

    if (Z_TYPE_P(post) == IS_ARRAY) {
        zend_update_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_POST), post);
    } else {
        linger_throw_exception(NULL, 0, "parameter nust be an array.");
        return;
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(linger_framework_request, setQuery)
{
    zval *query;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &query) == FAILURE) {
        return;
    }

    if (Z_TYPE_P(query) == IS_ARRAY) {
        zend_update_property(request_ce, getThis(), ZEND_STRL(REQUEST_PROPERTIES_QUERY), query);
    } else {
        linger_throw_exception(NULL, 0, "parameter must be an array.");
        return;
    }

    RETURN_ZVAL(getThis(), 1, 0);
}

zend_function_entry request_methods[] = {
    PHP_ME(linger_framework_request, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_request, getMethod, linger_framework_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, getUri, linger_framework_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, getQuery, linger_framework_request_get_default_filter_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, getPost, linger_framework_request_get_default_filter_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, getParam, linger_framework_request_get_default_filter_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, getFile, linger_framework_request_get_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, getCookie, linger_framework_request_get_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, isAjax, linger_framework_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, isPost, linger_framework_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, isGet, linger_framework_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, isCli, linger_framework_request_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, setMethod, linger_framework_request_set_method_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, setUri, linger_framework_request_set_uri_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, setParam, linger_framework_request_set_param_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, setPost, linger_framework_request_set_post_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_request, setQuery, linger_framework_request_set_query_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(request)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\Request", request_methods);
    request_ce = zend_register_internal_class_ex(&ce, NULL);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_INSTANCE), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC TSRMLS_CC);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_METHOD), ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_URI), ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_COOKIE), ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_QUERY), ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_PARAM), ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_POST), ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(request_ce, ZEND_STRL(REQUEST_PROPERTIES_FILES), ZEND_ACC_PROTECTED TSRMLS_CC);

    return SUCCESS;
}
