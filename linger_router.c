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
zend_class_entry *router_rule_ce;

#define LINGER_ROUTER_PROPERTIES_INSTANCE        "_instance"
#define LINGER_ROUTER_PROPERTIES_RULES           "_rules"

zval *linger_router_instance(zval *this TSRMLS_DC)
{
    zval *router_rules;
    zval *instance = zend_read_static_property(router_ce, ZEND_STRL(LINGER_ROUTER_PROPERTIES_INSTANCE), 1 TSRMLS_CC);

    if (Z_TYPE_P(instance) == IS_OBJECT &&
            instanceof_function(Z_OBJCE_P(instance), router_ce)) {
        return instance;
    }

    if (this) {
        instance = this;
    } else {
        instance = NULL;
        MAKE_STD_ZVAL(instance);
        object_init_ex(instance, router_ce);
    }

    MAKE_STD_ZVAL(router_rules);
    array_init(router_rules);
    zend_update_property(router_ce, instance, ZEND_STRL(LINGER_ROUTER_PROPERTIES_RULES), router_rules TSRMLS_CC);
    zend_update_static_property(router_ce, ZEND_STRL(LINGER_ROUTER_PROPERTIES_INSTANCE), instance TSRMLS_CC);
    zval_ptr_dtor(&router_rules);

    return instance;
}

zval *linger_router_match(zval *this, zval *request TSRMLS_DC)
{
    zval *rules = zend_read_property(router_ce, this, ZEND_STRL(LINGER_ROUTER_PROPERTIES_RULES), 1 TSRMLS_CC);

    if (IS_ARRAY == Z_TYPE_P(rules)) {
        HashTable *ht;
        ulong idx = 0;
        ht = Z_ARRVAL_P(rules);
        zval **router_rule = NULL;
        zval *zv_request_method, *zv_uri;
        zval *zv_replace_empty = NULL;
        char *lower_request_method = NULL;
        zend_bool is_find = 0;

        zval *curr_request_method = linger_request_get_request_method(request TSRMLS_CC);
        zval *curr_request_uri = linger_request_get_request_uri(request TSRMLS_CC);
        if (!curr_request_method || !curr_request_uri)
            return NULL;

        char *trimed_uri = php_trim(Z_STRVAL_P(curr_request_uri), Z_STRLEN_P(curr_request_uri), "/", 1, NULL, 3);
        char *format_uri = NULL;
        int format_uri_len = spprintf(&format_uri, 0, "/%s/", trimed_uri);

        linger_efree(trimed_uri);

        lower_request_method = zend_str_tolower_dup(Z_STRVAL_P(curr_request_method), Z_STRLEN_P(curr_request_method));

        MAKE_STD_ZVAL(zv_replace_empty);
        ZVAL_STRING(zv_replace_empty, "", 1);

        for (zend_hash_internal_pointer_reset(ht);
                zend_hash_has_more_elements(ht) == SUCCESS;
                zend_hash_move_forward(ht)) {

            if (zend_hash_get_current_data(ht, (void **)&router_rule) == FAILURE) {
                continue;
            }
            if (IS_OBJECT != Z_TYPE_PP(router_rule) ||
                    !instanceof_function(Z_OBJCE_PP(router_rule), router_rule_ce)) {
                continue;
            }

            zv_request_method = linger_router_rule_get_request_method(*router_rule TSRMLS_CC);
            if (zv_request_method && Z_TYPE_P(zv_request_method) == IS_STRING) {
                if (strncmp(lower_request_method, Z_STRVAL_P(zv_request_method), Z_STRLEN_P(zv_request_method))) {
                    continue;
                }

                zv_uri = linger_router_rule_get_uri(*router_rule TSRMLS_CC);
                if (zv_uri && IS_STRING == Z_TYPE_P(zv_uri)) {
                    pcre_cache_entry *pce_regexp;
                    zval matches, *subparts, *map;

                    if ((pce_regexp = pcre_get_compiled_regex_cache(ZEND_STRL("/@(.*?):/") TSRMLS_CC)) == NULL) {
                        continue;
                    }

                    MAKE_STD_ZVAL(map);
                    ZVAL_NULL(map);
                    php_pcre_match_impl(pce_regexp, Z_STRVAL_P(zv_uri), Z_STRLEN_P(zv_uri), &matches, map, 1, 0, 0, 0 TSRMLS_CC);
                    if (!zend_hash_num_elements(Z_ARRVAL_P(map))) {
                        zval_ptr_dtor(&map);
                        continue;
                    } else {

                        zval **params_map;

                        if (zend_hash_index_find(Z_ARRVAL_P(map), 1, (void **)&params_map) != SUCCESS) {
                            zval_ptr_dtor(&map);
                            continue;
                        }

                        char *tmp_uri = NULL;
                        int result_len = 0, replace_count = 0;
                        //PHPAPI char *php_pcre_replace_impl(pcre_cache_entry *pce, char *subject, int subject_len, zval *replace_value,
                        //    int is_callable_replace, int *result_len, int limit, int *replace_count TSRMLS_DC);
                        tmp_uri = php_pcre_replace_impl(pce_regexp, Z_STRVAL_P(zv_uri), Z_STRLEN_P(zv_uri), zv_replace_empty, 0, &result_len, -1, &replace_count TSRMLS_CC);

                        zval *params;
                        MAKE_STD_ZVAL(params);
                        ZVAL_NULL(params);
                        int reg_len;
                        char *reg = NULL;
                        pcre_cache_entry *pce_regexp_t;

                        reg_len = spprintf(&reg, 0, "#^%s$#", tmp_uri);

                        if ((pce_regexp_t = pcre_get_compiled_regex_cache(reg, reg_len TSRMLS_CC)) == NULL) {
                            linger_efree(reg);
                            continue;
                        }

                        php_pcre_match_impl(pce_regexp_t, format_uri, format_uri_len, &matches, params, 0, 0, 0, 0 TSRMLS_CC);
                        if (!zend_hash_num_elements(Z_ARRVAL_P(params))) {
                            zval_ptr_dtor(&params);
                            linger_efree(reg);
                            continue;
                        }

                        linger_efree(reg);

                        zval *ret, **name, **ppzval;
                        char *key = NULL;
                        uint len = 0;
                        ulong index = 1;
                        HashTable *hashtable;

                        hashtable = Z_ARRVAL_P(params);
                        MAKE_STD_ZVAL(ret);
                        array_init(ret);

                        for (zend_hash_internal_pointer_reset(hashtable);
                                zend_hash_has_more_elements(hashtable) == SUCCESS;
                                zend_hash_move_forward(hashtable)) {

                            if (zend_hash_get_current_data(hashtable, (void **)&ppzval) == FAILURE) {
                                continue;
                            }

                            if (zend_hash_get_current_key_ex(hashtable, &key, &len, &index, 0, NULL) == HASH_KEY_IS_LONG) {
                                if (*params_map && zend_hash_index_find(Z_ARRVAL_PP(params_map), index - 1, (void **)&name) == SUCCESS
                                        && Z_TYPE_PP(name) == IS_STRING) {
                                    Z_ADDREF_P(*ppzval);
                                    zend_hash_update(Z_ARRVAL_P(ret), Z_STRVAL_PP(name), Z_STRLEN_PP(name) + 1, (void **)ppzval, sizeof(zval *), NULL);
                                }
                            } else {
                                Z_ADDREF_P(*ppzval);
                                zend_hash_update(Z_ARRVAL_P(ret), key, len, (void **)ppzval, sizeof(zval *), NULL);
                            }
                        }

                        linger_request_set_params(request, ret TSRMLS_CC);
                        zval_ptr_dtor(&map);
                        zval_ptr_dtor(&params);
                        zval_ptr_dtor(&ret);
                        is_find = 1;
                        break;
                    }
                } else {
                    continue;
                }
            } else {
                continue;
            }
        }

        linger_efree(lower_request_method);
        linger_efree(format_uri);
        zval_ptr_dtor(&zv_replace_empty);
        if (is_find && *router_rule != NULL && router_rule != NULL)  {
            return *router_rule;
        }
    }

    return NULL;
}

void linger_router_add_rule(zval *this, zval *rule TSRMLS_DC)
{
    zval *rules = zend_read_property(router_ce, this, ZEND_STRL(LINGER_ROUTER_PROPERTIES_RULES), 1 TSRMLS_CC);
    /* add refcount, that's important! */
    Z_ADDREF_P(rule);
    add_next_index_zval(rules, rule);
    zend_update_property(router_ce, this, ZEND_STRL(LINGER_ROUTER_PROPERTIES_RULES), rules TSRMLS_CC);
}

PHP_METHOD(linger_framework_router, __construct)
{

}

PHP_METHOD(linger_framework_router, add)
{
    zval *rule_item;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &rule_item) == FAILURE) {
        return;
    }

    if (IS_OBJECT == Z_TYPE_P(rule_item) &&
            instanceof_function(Z_OBJCE_P(rule_item), router_rule_ce)) {

        linger_router_add_rule(getThis(), rule_item TSRMLS_CC);

        RETURN_ZVAL(getThis(), 1, 0);
    } else {
        linger_throw_exception(NULL, 0, "parameter must be a instance of class %s.", router_rule_ce->name);

        RETURN_FALSE;
    }
}

PHP_METHOD(linger_framework_router, get)
{
    zval *uri, *class, *class_method;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &uri, &class, &class_method) == FAILURE) {
        return;
    }

    zval *request_method;
    MAKE_STD_ZVAL(request_method);
    ZVAL_STRING(request_method,"get", 0);
    zval *rule = linger_router_rule_instance(NULL, request_method, uri, class, class_method);
    linger_router_add_rule(getThis(), rule TSRMLS_CC);

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(linger_framework_router, post)
{
    zval *uri, *class, *class_method;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &uri, &class, &class_method) == FAILURE) {
        return;
    }

    zval *request_method;
    MAKE_STD_ZVAL(request_method);
    ZVAL_STRING(request_method, "post", 0);
    zval *rule = linger_router_rule_instance(NULL, request_method, uri, class, class_method);
    linger_router_add_rule(getThis(), rule TSRMLS_CC);

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(linger_framework_router, put)
{
    zval *uri, *class, *class_method;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &uri, &class, &class_method) == FAILURE) {
        return;
    }

    zval *request_method;
    MAKE_STD_ZVAL(request_method);
    ZVAL_STRING(request_method, "put", 0);
    zval *rule = linger_router_rule_instance(NULL, request_method, uri, class, class_method);
    linger_router_add_rule(getThis(), rule TSRMLS_CC);

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(linger_framework_router, delete)
{
    zval *uri, *class, *class_method;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "zzz", &uri, &class, &class_method) == FAILURE) {
        return;
    }

    zval *request_method;
    MAKE_STD_ZVAL(request_method);
    ZVAL_STRING(request_method, "delete", 0);
    zval *rule = linger_router_rule_instance(NULL, request_method, uri, class, class_method);
    linger_router_add_rule(getThis(), rule TSRMLS_CC);

    RETURN_ZVAL(getThis(), 1, 0);
}

zend_function_entry router_methods[] = {
    PHP_ME(linger_framework_router, __construct, NULL, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_router, add,         NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router, get,         NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router, post,        NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router, put,         NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router, delete,      NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(router)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\Router", router_methods);
    router_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(router_ce, ZEND_STRL(LINGER_ROUTER_PROPERTIES_INSTANCE), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);
    zend_declare_property_null(router_ce, ZEND_STRL(LINGER_ROUTER_PROPERTIES_RULES), ZEND_ACC_PROTECTED);
    return SUCCESS;
}
