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
#include "php_linger_framework.h"
#include "linger_response.h"
#include "php_output.h"
#include "SAPI.h"

#include "zend_smart_str.h"
#include "ext/json/php_json.h"

zend_class_entry *response_ce;

ZEND_BEGIN_ARG_INFO_EX(linger_framework_response_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(linger_framework_response_header_arginfo, 0, 0, 2)
ZEND_ARG_INFO(0, key)
ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(linger_framework_response_status_arginfo, 0, 0, 0)
ZEND_ARG_INFO(0, status)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(linger_framework_response_body_arginfo, 0, 0, 0)
ZEND_ARG_INFO(0, body)
ZEND_END_ARG_INFO()

zval *linger_response_instance(zval *this)
{
    zval *instance = zend_read_static_property(response_ce, ZEND_STRL(RESPONSE_PROPERTIES_INSTANCE), 1);

    if (instance && IS_OBJECT == Z_TYPE_P(instance) &&
            instanceof_function(Z_OBJCE_P(instance), response_ce)) {
        return instance;
    }

    if (Z_ISUNDEF_P(this)) {
        object_init_ex(this, response_ce);
    }

    zval headers = {{0}};
    array_init(&headers);
    zend_update_property(response_ce, this, ZEND_STRL(RESPONSE_PROPERTIES_HEADER), &headers);
    zval_ptr_dtor(&headers);

    zval status = {{0}};
    ZVAL_LONG(&status, 200);
    zend_update_property(response_ce, this, ZEND_STRL(RESPONSE_PROPERTIES_STATUS), &status);
    zval_ptr_dtor(&status);

    zval body = {{0}};
    ZVAL_STRING(&body, "");
    zend_update_property(response_ce, this, ZEND_STRL(RESPONSE_PROPERTIES_BODY), &body);
    zval_ptr_dtor(&body);

    zend_update_static_property(response_ce, ZEND_STRL(RESPONSE_PROPERTIES_INSTANCE), this);

    return this;
}

void linger_response_send(zval *this)
{
    zval *headers = zend_read_property(response_ce, this, ZEND_STRL(RESPONSE_PROPERTIES_HEADER), 1, NULL);
    zval *body = zend_read_property(response_ce, this, ZEND_STRL(RESPONSE_PROPERTIES_BODY), 1, NULL);
    zval *status = zend_read_property(response_ce, this, ZEND_STRL(RESPONSE_PROPERTIES_STATUS), 1, NULL);

    SG(sapi_headers).http_response_code = (int)Z_LVAL_P(status);

    if (!SG(headers_sent)) {
        zval *header = NULL;
        zend_string *key = NULL;
        ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARRVAL_P(headers), key, header) {
            char *line;
            int len = spprintf(&line, 0, "%s:%s", ZSTR_VAL(key), Z_STRVAL_P(header));
            sapi_header_line ctr = {
                .line = line,
                .line_len = (uint32_t)len,
            };
            sapi_header_op(SAPI_HEADER_REPLACE, &ctr);
            linger_efree(line);
        }
        ZEND_HASH_FOREACH_END();
    }

    PHPWRITE(Z_STRVAL_P(body), Z_STRLEN_P(body));
}

static int check_response_status(int status)
{
    return (status == 100 || status == 101 || status == 200 || status == 201
            || status == 202 || status == 203 || status == 204 || status == 205
            || status == 206 || status == 300 || status == 301 || status == 302
            || status == 303 || status == 304 || status == 305 || status == 400
            || status == 401 || status == 402 || status == 403 || status == 404
            || status == 405 || status == 406 || status == 407 || status == 408
            || status == 409 || status == 410 || status == 411 || status == 412
            || status == 413 || status == 414 || status == 415 || status == 500
            || status == 501 || status == 502 || status == 503 || status == 504
            || status == 505);
}

int linger_response_set_status(zval *this, zval *status)
{
    if (status && IS_LONG == Z_TYPE_P(status)) {
        int l_status = (int)Z_LVAL_P(status);
        if (check_response_status(l_status)) {
            zend_update_property(response_ce, this, ZEND_STRL(RESPONSE_PROPERTIES_STATUS), status);
            return SUCCESS;
        } else {
            linger_throw_exception(NULL, 0, "invalid status code.");
            return FAILURE;
        }
    } else {
        linger_throw_exception(NULL, 0, "the response status code must be integer.");
        return FAILURE;
    }
}

int linger_response_set_header(zval *this, zend_string *key, zval *val)
{
    if (val && IS_STRING == Z_TYPE_P(val)) {
        zval *headers = zend_read_property(response_ce, this, ZEND_STRL(RESPONSE_PROPERTIES_HEADER), 1, NULL);
        zend_hash_add(Z_ARRVAL_P(headers), key, val);
        return SUCCESS;
    } else {
        linger_throw_exception(NULL, 0, "the parameter 2 must be string.");
        return FAILURE;
    }
}

int linger_response_set_body(zval *this, zval *body)
{
    if (body && IS_STRING == Z_TYPE_P(body)) {
        zend_update_property(response_ce, this, ZEND_STRL(RESPONSE_PROPERTIES_BODY), body);
        return SUCCESS;
    } else {
        linger_throw_exception(NULL, 0, "the response body must be string.");
        return FAILURE;
    }
}

PHP_METHOD(linger_framework_response, __construct)
{

}

PHP_METHOD(linger_framework_response, status)
{
    zval *status = NULL;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &status) == FAILURE) {
        return;
    }

    zval *this = getThis();

    if (linger_response_set_status(this, status) == SUCCESS) {
        RETURN_ZVAL(this, 1, 0);
    }
}

PHP_METHOD(linger_framework_response, header)
{
    zend_string *key = NULL;
    zval *val = NULL;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "Sz", &key, &val) == FAILURE) {
        return;
    }

    zval *this = getThis();

    if (linger_response_set_header(this, key, val) == SUCCESS) {
        RETURN_ZVAL(this, 1, 0);
    }
}

PHP_METHOD(linger_framework_response, body)
{
    zval *body;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &body) == FAILURE) {
        return;
    }

    zval *this = getThis();

    if (linger_response_set_body(this, body) == SUCCESS) {
        RETURN_ZVAL(this, 1, 0);
    }
}

PHP_METHOD(linger_framework_response, send)
{
    linger_response_send(getThis());
}

PHP_METHOD(linger_framework_response, json)
{
    zval *obj;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &obj) == FAILURE) {
        return;
    }

    zval *this = getThis();
    smart_str buf = {0};
    php_json_encode(&buf, obj, 0);
    if (JSON_G(error_code) != PHP_JSON_ERROR_NONE) {
        smart_str_free(&buf);
    } else {
        zend_string *key = zend_string_init("Content-Type", 12, 0);
        zval val = {{0}};
        ZVAL_STRING(&val, "application/json;charset=utf-8");
        linger_response_set_header(this, key, &val);
        zval body = {{0}};

        smart_str_0(&buf); /* copy? */
        if (buf.s) {
            ZVAL_STRINGL(&body, (buf.s)->val, (buf.s)->len);
        }

        linger_response_set_body(this, &body);
    }

    RETURN_ZVAL(this, 1, 0);
}

zend_function_entry response_methods[] = {
    PHP_ME(linger_framework_response, __construct, linger_framework_response_void_arginfo, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_response, status, linger_framework_response_status_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_response, header, linger_framework_response_header_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_response, body, linger_framework_response_body_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_response, send, linger_framework_response_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_response, json, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(response)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\Response", response_methods);
    response_ce = zend_register_internal_class_ex(&ce, NULL);
    zend_declare_property_null(response_ce, ZEND_STRL(RESPONSE_PROPERTIES_INSTANCE), ZEND_ACC_STATIC | ZEND_ACC_PROTECTED);
    zend_declare_property_null(response_ce, ZEND_STRL(RESPONSE_PROPERTIES_HEADER), ZEND_ACC_PROTECTED);
    zend_declare_property_null(response_ce, ZEND_STRL(RESPONSE_PROPERTIES_STATUS), ZEND_ACC_PROTECTED);
    zend_declare_property_null(response_ce, ZEND_STRL(RESPONSE_PROPERTIES_BODY), ZEND_ACC_PROTECTED);

    return SUCCESS;
}
