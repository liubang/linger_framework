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
#include "ext/standard/php_var.h"
#include "php_linger_framework.h"
#include "linger_router.h"
#include "linger_router_rule.h"
#include "linger_request.h"

#define PREG_STR "~(?:@(.*?):)~x"
#define CHUNK_PREG_LEN 1024

zend_class_entry *router_ce;

ZEND_BEGIN_ARG_INFO_EX(linger_framework_router_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(linger_framework_router_set_chunk_size_arginfo, 0, 0, 1)
ZEND_ARG_INFO(0, chunkSize)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(linger_framework_router_add_arginfo, 0, 0, 1)
ZEND_ARG_OBJ_INFO(0, rule_item, Linger\\Framework\\RouterRule, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(linger_framework_router_3_arginfo, 0, 0, 3)
ZEND_ARG_INFO(0, uri)
ZEND_ARG_INFO(0, class)
ZEND_ARG_INFO(0, method)
ZEND_END_ARG_INFO()

typedef struct _metadata {
    zend_long curr_chunk;
    zend_long curr_num;
    zend_long max_index;
} metadata;

typedef struct _router_obj {
    metadata *md;
    zend_object std;
} router_obj;

#define Z_GET_ROUTER_OBJ(zv) \
    (router_obj *)((char *)Z_OBJ_P((zv)) - XtOffsetOf(router_obj, std))

#define ROUTER_OBJ_GET_MD(ro) \
    (ro)->md

#define Z_GET_MD(zv) \
    ROUTER_OBJ_GET_MD(Z_GET_ROUTER_OBJ(zv))

zend_object_handlers linger_router_obj_handlers;

static zend_object *linger_create_router_obj(zend_class_entry *ce) /* {{{ */
{
    router_obj *internal;
    metadata *md = emalloc(sizeof(metadata));
    md->curr_chunk = 0;
    md->curr_num = 1;
    md->max_index = 1;

    internal = ecalloc(1, sizeof(router_obj) + zend_object_properties_size(ce));
    internal->md = md;

    zend_object_std_init(&internal->std, ce);
    object_properties_init(&internal->std, ce);
    internal->std.handlers = &linger_router_obj_handlers;

    return &internal->std;
} /* }}} */

static void linger_free_router_obj(zend_object *object) /* {{{ */
{
    router_obj *internal = (router_obj *)((char *)object - XtOffsetOf(router_obj, std));

    zend_object_std_dtor(&internal->std);
    linger_efree(internal->md);
} /* }}} */

zval *linger_router_instance(zval *this) /* {{{ */
{
    zval *instance = zend_read_static_property(router_ce, ZEND_STRL(LINGER_ROUTER_PROPERTIES_INSTANCE), 1);
    if (Z_TYPE_P(instance) == IS_OBJECT &&
            instanceof_function(Z_OBJCE_P(instance), router_ce TSRMLS_CC)) {
        return instance;
    }
    if (Z_ISUNDEF_P(this)) {
        object_init_ex(this, router_ce);
    }

    zval router_rules = {{0}};
    array_init(&router_rules);
    zend_update_property(router_ce, this, ZEND_STRL(LINGER_ROUTER_PROPERTIES_RULES), &router_rules);
    zval_ptr_dtor(&router_rules);

    zend_update_property_long(router_ce, this, ZEND_STRL(LINGER_ROUTER_PROPERTIES_CHUNK_SIZE), (zend_long)10);

    return this;
} /* }}} */

zval *linger_router_match(zval *this, zval *request) /* {{{ */
{
    zval *rules = zend_read_property(router_ce, this, ZEND_STRL(LINGER_ROUTER_PROPERTIES_RULES), 1, NULL);

    if (UNEXPECTED(IS_ARRAY != Z_TYPE_P(rules))) {
        return NULL;
    }

    zval *curr_request_method = linger_request_get_request_method(request);
    zval *curr_request_uri = linger_request_get_request_uri(request);
    if (!curr_request_method || !curr_request_uri) {
        return NULL;
    }

    int inc = 1;
    zval *chunk;
    zval *matched_rule;
    int is_find;
    int max_preg_len;

start:

    max_preg_len = CHUNK_PREG_LEN * inc;
    is_find = 0;

    ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(rules), chunk) {
        zval *rule;
        char *preg = emalloc(sizeof(char) * max_preg_len);
        memcpy(preg, "~^(?", 4);
        int preg_len = 4;
        if (UNEXPECTED(IS_ARRAY != Z_TYPE_P(chunk))) {
            continue;
        }
        ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(chunk), rule) {
            zend_string *compiled_uri = linger_router_rule_get_compiled_uri(rule);            
            zval *request_method = linger_router_rule_get_request_method(rule);
            if (!compiled_uri) {
                zval *request_uri = linger_router_rule_get_uri(rule); 
                compiled_uri = Z_STR_P(request_uri);
            }
            if (!strncasecmp(Z_STRVAL_P(curr_request_method), Z_STRVAL_P(request_method), Z_STRLEN_P(curr_request_method))) {
                if (preg_len > max_preg_len) {
                    linger_efree(preg);
                    inc++;
                    goto start;
                }
                memcpy(preg + preg_len, "|", 1);
                memcpy(preg + preg_len + 1, ZSTR_VAL(compiled_uri), ZSTR_LEN(compiled_uri));
                preg_len += (1 + ZSTR_LEN(compiled_uri));
            }
        } ZEND_HASH_FOREACH_END();
        if (preg_len == 0) {
            linger_efree(preg);
            continue;
        }
        memcpy(preg + preg_len, ")$~x\0", 5);
        preg_len += 4;
        zend_string *zs_preg = zend_string_init(preg, preg_len, 0);
        linger_efree(preg);
        pcre_cache_entry *pce_regexp_p = NULL;
        if ((pce_regexp_p = pcre_get_compiled_regex_cache(zs_preg)) == NULL) {
            zend_string_release(zs_preg);
            continue;
        }
        zend_string_release(zs_preg);
        zval params = {{0}};
        zval matches = {{0}};
        pce_regexp_p->refcount++;
        php_pcre_match_impl(pce_regexp_p, Z_STRVAL_P(curr_request_uri), (int)Z_STRLEN_P(curr_request_uri),
                                    &matches, &params, 0, 0, 0, 0);
        pce_regexp_p->refcount--;

        if (IS_FALSE == Z_TYPE(matches) || (IS_LONG == Z_TYPE(matches) && Z_LVAL(matches) == 0)) {
            zval_ptr_dtor(&matches);
            zval_ptr_dtor(&params);
            continue;
        }
        zend_long index = zend_array_count(Z_ARRVAL(params));
        if ((matched_rule = zend_hash_index_find(Z_ARRVAL_P(chunk), index)) == NULL) {
            zval_ptr_dtor(&matches);
            zval_ptr_dtor(&params);
            continue;
        }
        zval *params_map = linger_router_rule_get_params_map(matched_rule);
        if (params_map && IS_ARRAY == Z_TYPE_P(params_map)) {
            zval format_params = {{0}};
            array_init(&format_params);
            zval *param_name;
            zend_ulong i; 
            ZEND_HASH_FOREACH_NUM_KEY_VAL(Z_ARRVAL_P(params_map), i, param_name) {
                zval *param_val = zend_hash_index_find(Z_ARRVAL(params), i + 1); 
                add_assoc_zval_ex(&format_params, Z_STRVAL_P(param_name), Z_STRLEN_P(param_name), param_val);
                Z_TRY_ADDREF_P(param_val);
            } ZEND_HASH_FOREACH_END();
            linger_request_set_params(request, &format_params);
            zval_ptr_dtor(&format_params);
        } 
        zval_ptr_dtor(&params);
        zval_ptr_dtor(&matches);
        is_find = 1;
        break;
    } ZEND_HASH_FOREACH_END();
    
    if (is_find) {
        return matched_rule;
    } else {
        return NULL;
    }
}
/* }}} */

static void linger_router_add_rule(zval *this, zval *rule_item) /* {{{ */
{
    if (EXPECTED(IS_OBJECT == Z_TYPE_P(rule_item)
                 && instanceof_function(Z_OBJCE_P(rule_item), router_rule_ce))) {
        zval *rules = zend_read_property(router_ce, this, ZEND_STRL(LINGER_ROUTER_PROPERTIES_RULES), 1, NULL);
        zval *chunk_size = zend_read_property(router_ce, this, ZEND_STRL(LINGER_ROUTER_PROPERTIES_CHUNK_SIZE), 1, NULL);

        metadata *md = Z_GET_MD(this);

        if (md->curr_num > Z_LVAL_P(chunk_size)) {
            md->max_index = 1;
            md->curr_num = 1;
            md->curr_chunk++;
        }

        md->curr_num++;

        zval *zv_uri = linger_router_rule_get_uri(rule_item);
        zval *inter_arr_p;

        if (NULL == strchr(Z_STRVAL_P(zv_uri), '@')) {
            goto ed;          
        }

        zend_string *preg_str = zend_string_init(ZEND_STRL(PREG_STR), 0);
        zend_string *zs_empty = zend_string_init("", 0, 0);
        zval zv_empty = {{0}};
        ZVAL_STRING(&zv_empty, "");

        pcre_cache_entry *pce_regexp;
        zval matches = {{0}},
             map = {{0}};
             //zv_compiled_uri = {{0}};

        if ((pce_regexp = pcre_get_compiled_regex_cache(preg_str)) != NULL) {
            pce_regexp->refcount++;
            php_pcre_match_impl(pce_regexp, Z_STRVAL_P(zv_uri), (int)Z_STRLEN_P(zv_uri), &matches, &map, 1, 0, 0, 0);
            pce_regexp->refcount--;
            if (IS_FALSE == Z_TYPE(matches) || (IS_LONG == Z_TYPE(matches) && Z_LVAL(matches) == 0)) {
                zval_ptr_dtor(&map);
                zval_ptr_dtor(&matches);
            } else {
                zval *params_map = NULL;
                if ((params_map = zend_hash_index_find(Z_ARRVAL(map), 1)) == NULL) {
                    zval_ptr_dtor(&map);
                    zval_ptr_dtor(&matches);
                } else {
                    linger_router_rule_set_params_map(rule_item, params_map);
                    zend_long cnt = zend_array_count(Z_ARRVAL_P(params_map)) + 1;
                    md->max_index = md->max_index > cnt ? md->max_index : cnt;
                     
                    pce_regexp->refcount++;
#if PHP_API_VERSION <= 20160303
                    zend_string *compiled_uri = php_pcre_replace_impl(pce_regexp, Z_STR_P(zv_uri), Z_STRVAL_P(zv_uri),
                                           Z_STRLEN_P(zv_uri), &zv_empty, 0, -1, NULL);
#else
                    zend_string *compiled_uri = php_pcre_replace_impl(pce_regexp, Z_STR_P(zv_uri), Z_STRVAL_P(zv_uri),
                                           Z_STRLEN_P(zv_uri), zs_empty, -1, NULL);
#endif
                    pce_regexp->refcount--;

                    zend_long rep = md->max_index - cnt;
                    if (rep > 0) {
                        zend_string *zs_repeat = zend_string_safe_alloc(2, rep, 0, 0);
                        size_t zs_repeat_len = 2 * rep;
                        char *s, *e, *ee;
                        ptrdiff_t l=0; 
                        memcpy(ZSTR_VAL(zs_repeat), "()", 2);
                        s = ZSTR_VAL(zs_repeat);
                        e = ZSTR_VAL(zs_repeat) + 2;
                        ee = ZSTR_VAL(zs_repeat) + zs_repeat_len; 
                        while (e < ee) { 
                            l = (e-s) < (ee-e) ? (e-s) : (ee-e);
                            memmove(e, s, l);
                            e += l;
                        }
                        ZSTR_VAL(zs_repeat)[zs_repeat_len] = '\0';
                        
                        char *tmp_char = emalloc(sizeof(char) * (ZSTR_LEN(zs_repeat) + ZSTR_LEN(compiled_uri) + 1));
                        memcpy(tmp_char, ZSTR_VAL(compiled_uri), ZSTR_LEN(compiled_uri));
                        memcpy(tmp_char + ZSTR_LEN(compiled_uri), ZSTR_VAL(zs_repeat), ZSTR_LEN(zs_repeat) + 1);
                        //ZVAL_STRING(&zv_compiled_uri, tmp_char);
                        (void)linger_router_rule_set_compiled_uri(rule_item, zend_string_init(tmp_char, ZSTR_LEN(zs_repeat) + ZSTR_LEN(compiled_uri), 0));
                        linger_efree(tmp_char);
                        zend_string_release(zs_repeat);
                        zend_string_release(compiled_uri);
                    } else {
                        //ZVAL_STRING(&zv_compiled_uri, ZSTR_VAL(compiled_uri));
                        (void)linger_router_rule_set_compiled_uri(rule_item, compiled_uri);
                    }
                      
                    //(void)linger_router_rule_set_compiled_uri(rule_item, &zv_compiled_uri);
                    //zval_ptr_dtor(&zv_compiled_uri);
                    //zend_string_release(compiled_uri);
                    
                    zval_ptr_dtor(&map);
                    zval_ptr_dtor(&matches);
                }
            }    
        }

        zval_ptr_dtor(&zv_empty);
        zend_string_release(zs_empty);
        zend_string_release(preg_str);

ed:

        if ((inter_arr_p = zend_hash_index_find(Z_ARRVAL_P(rules), md->curr_chunk)) == NULL) {
            zval inter_arr = {{0}};
            array_init(&inter_arr); 
            add_index_zval(rules, md->curr_chunk, &inter_arr);
            inter_arr_p = &inter_arr;
        } 

        add_index_zval(inter_arr_p, md->max_index, rule_item);

        md->max_index++;

    } else {
        linger_throw_exception(NULL, 0, "parameter must be a instance of class %s.", router_rule_ce->name);
    }
}
/* }}} */

PHP_METHOD(linger_framework_router, __construct) /* {{{ */
{

}
/* }}} */

PHP_METHOD(linger_framework_router, setChunkSize) /* {{{ */
{
    zend_long chunkSize;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "l", &chunkSize) == FAILURE) {
        return;
    }

    zval *this = getThis();
    zend_update_property_long(router_ce, this, ZEND_STRL(LINGER_ROUTER_PROPERTIES_CHUNK_SIZE), chunkSize);

    RETURN_ZVAL(this, 1, 0);
}
/* }}} */

PHP_METHOD(linger_framework_router, add) /* {{{ */ 
{
    zval *rule_item;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &rule_item) == FAILURE) {
        return;
    }

    zval *self = getThis();
    Z_ADDREF_P(rule_item);
    linger_router_add_rule(self, rule_item);

    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

PHP_METHOD(linger_framework_router, get) /* {{{ */
{
    zval *uri = NULL,
         *class = NULL,
         *method = NULL;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zzz", &uri, &class, &method) == FAILURE) {
        return;
    }
    zval req_method = {{0}};
    ZVAL_STRING(&req_method, "get");
    zval rule = {{0}};
    (void)linger_router_rule_instance(&rule, &req_method, uri, class, method);
    zval_ptr_dtor(&req_method);
    zval *self = getThis();
    linger_router_add_rule(self, &rule);
    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

PHP_METHOD(linger_framework_router, post) /* {{{ */
{
    zval *uri = NULL,
         *class = NULL,
         *method = NULL;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zzz", &uri, &class, &method) == FAILURE) {
        return;
    }
    zval req_method = {{0}};
    ZVAL_STRING(&req_method, "post");
    zval rule = {{0}};
    (void)linger_router_rule_instance(&rule, &req_method, uri, class, method);
    zval_ptr_dtor(&req_method);
    zval *self = getThis();
    linger_router_add_rule(self, &rule);
    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

PHP_METHOD(linger_framework_router, put) /* {{{ */
{
    zval *uri = NULL,
         *class = NULL,
         *method = NULL;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zzz", &uri, &class, &method) == FAILURE) {
        return;
    }
    zval req_method = {{0}};
    ZVAL_STRING(&req_method, "put");
    zval rule = {{0}};
    (void)linger_router_rule_instance(&rule, &req_method, uri, class, method);
    zval_ptr_dtor(&req_method);
    zval *self = getThis();
    linger_router_add_rule(self, &rule);
    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

PHP_METHOD(linger_framework_router, delete) /* {{{ */
{
    zval *uri = NULL,
         *class = NULL,
         *method = NULL;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "zzz", &uri, &class, &method) == FAILURE) {
        return;
    }
    zval req_method = {{0}};
    ZVAL_STRING(&req_method, "delete");
    zval rule = {{0}};
    (void)linger_router_rule_instance(&rule, &req_method, uri, class, method);
    zval_ptr_dtor(&req_method);
    zval *self = getThis();
    linger_router_add_rule(self, &rule);
    RETURN_ZVAL(self, 1, 0);
}
/* }}} */

PHP_METHOD(linger_framework_router, dump) /* {{{ */ 
{
    metadata *md = Z_GET_MD(getThis()); 
    php_printf("md:{curr_chunk:%d,curr_num:%d,max_index:%d}\n", md->curr_chunk, md->curr_num, md->max_index);
}
/* }}} */

zend_function_entry router_methods[] = {
    PHP_ME(linger_framework_router, __construct,linger_framework_router_void_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_router, add, linger_framework_router_add_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router, get, linger_framework_router_3_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router, put, linger_framework_router_3_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router, post, linger_framework_router_3_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router, delete, linger_framework_router_3_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router, setChunkSize, linger_framework_router_set_chunk_size_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_router, dump, linger_framework_router_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(router)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\Router", router_methods);
    router_ce = zend_register_internal_class_ex(&ce, NULL);
    router_ce->create_object = linger_create_router_obj;
    memcpy(&linger_router_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    linger_router_obj_handlers.offset = XtOffsetOf(router_obj, std);
    linger_router_obj_handlers.dtor_obj = zend_objects_destroy_object;
    linger_router_obj_handlers.free_obj = linger_free_router_obj;

    zend_declare_property_null(router_ce, ZEND_STRL(LINGER_ROUTER_PROPERTIES_INSTANCE), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);
    zend_declare_property_null(router_ce, ZEND_STRL(LINGER_ROUTER_PROPERTIES_RULES), ZEND_ACC_PRIVATE);
    zend_declare_property_null(router_ce, ZEND_STRL(LINGER_ROUTER_PROPERTIES_CHUNK_SIZE), ZEND_ACC_PRIVATE);

    return SUCCESS;
}
