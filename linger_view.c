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

zend_class_entry *view_ce;

#define VIEW_PROPERTIES_VARS   "_vars"
#define VIEW_PROPERTIES_TPLDIR "_tpl_dir"

zval *linger_view_instance(TSRMLS_DC)
{
    zval *instance = NULL;
    MAKE_STD_ZVAL(instance);
    object_init_ex(instance, view_ce);
    zval *vars = NULL;
    MAKE_STD_ZVAL(vars);
    array_init(vars);
    zend_update_property(view_ce, instance, ZEND_STRL(VIEW_PROPERTIES_VARS), vars TSRMLS_CC);
    zval *tpl_dir = NULL;
    zval_ptr_dtor(&vars);
    return instance;
}

static int linger_view_extract(zval *vars TSRMLS_DC)
{
    zval **entry;
    char *var_name;
    uint var_name_len;
    ulong num_key;
    HashPosition pos;

    if (vars && Z_TYPE_P(vars) == IS_ARRAY) {
        for (zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(vars), &pos);
                zend_hash_get_current_data_ex(Z_ARRVAL_P(vars), (void **)&entry, &pos) == SUCCESS;
                zend_hash_move_forward_ex(Z_ARRVAL_P(vars), &pos)) {
            if (zend_hash_get_current_key_ex(Z_ARRVAL_P(vars), &var_name, &var_name_len, &num_key, 0, &pos) != HASH_KEY_IS_STRING) {
                continue;
            }
            if (var_name_len == sizeof("GLOBALS") && !strcmp(var_name, "GLOBALS")) {
                continue;
            }
            if (var_name_len == sizeof("this") && !strcmp(var_name, "this") && EG(scope) && EG(scope)->name_length != 0) {
                continue;
            }
            ZEND_SET_SYMBOL_WITH_LENGTH(EG(active_symbol_table), var_name, var_name_len, *entry, Z_REFCOUNT_P(*entry) + 1, PZVAL_IS_REF(*entry));
        }
    }

    return SUCCESS;
}

#define RESTORE_ACTIVE_SYMBOL_TABLE() \
    do {\
        if (scope_vars_table) { \
            zend_hash_destroy(EG(active_symbol_table)); \
            FREE_HASHTABLE(EG(active_symbol_table)); \
            EG(active_symbol_table) = scope_vars_table; \
        } \
    } while(0)

int linger_view_render(zval *this, zval *tpl, zval *ret TSRMLS_DC)
{
    zval *vars;
    char *script;
    uint len;
    HashTable *scope_vars_table = NULL;
    if (Z_TYPE_P(tpl) != IS_STRING) {
        return FAILURE;
    }
    ZVAL_NULL(ret);
    vars = zend_read_property(view_ce, this, ZEND_STRL(VIEW_PROPERTIES_VARS), 1 TSRMLS_CC);
    if (EG(active_symbol_table)) {
        scope_vars_table = EG(active_symbol_table);
    }
    ALLOC_HASHTABLE(EG(active_symbol_table));
    zend_hash_init(EG(active_symbol_table), 0, NULL, ZVAL_PTR_DTOR, 0);
    (void)linger_view_extract(vars TSRMLS_CC);
    
    if (php_output_start_user(NULL, 0, PHP_OUTPUT_HANDLER_STDFLAGS TSRMLS_CC) == FAILURE) {
        php_error_docref("ref.outcontrol" TSRMLS_CC, E_WARNING, "failed to create buffer");
        return FAILURE;
    }
    if (IS_ABSOLUTE_PATH(Z_STRVAL_P(tpl), Z_STRLEN_P(tpl))) {
        script = Z_STRVAL_P(tpl);
        len = Z_STRLEN_P(tpl);
        if (linger_application_import(script, len + 1, 0 TSRMLS_CC) == FAILURE) {
            php_output_end(TSRMLS_C);
            RESTORE_ACTIVE_SYMBOL_TABLE();
            zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "failed opening template %s: %s", script, strerror(errno));
            return FAILURE;
        }
    } else {
        zval *tpl_dir = zend_read_property(view_ce, this, ZEND_STRL(VIEW_PROPERTIES_TPLDIR), 0 TSRMLS_CC);
        if (Z_TYPE_P(tpl_dir) != IS_STRING) {
            if (LINGER_FRAMEWORK_G(view_directory)) {
                len = spprintf(&script, 0, "%s%c%s", LINGER_FRAMEWORK_G(view_directory), '/', Z_STRVAL_P(tpl));
            } else {
                php_output_end(TSRMLS_C);
                RESTORE_ACTIVE_SYMBOL_TABLE();
                zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "Could not determine the view script path, call %s::setScriptPath to specific it", view_ce->name);
                return FAILURE;
            }
        } else {
            len = spprintf(&script, "%s%c%s", Z_STRVAL_P(tpl_dir), '/', Z_STRVAL_P(tpl));
        }
        if (linger_application_import(script, len + 1, 0 TSRMLS_CC) == 0) {
            php_output_end(TSRMLS_C);
            RESTORE_ACTIVE_SYMBOL_TABLE();
            zend_throw_excpetion_ex(NULL, 0 TSRMLS_CC, "failed opening template %s:%s", script, strerror(errno));
            linger_efree(script);
            return FAILURE;
        }
        linger_efree(script);
    }

    RESTORE_ACTIVE_SYMBOL_TABLE();

#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 4))
    CG(short_tags) = short_open_tag;
    if (buffer->len) {
        ZVAL_STRINGL(ret, buffer->buffer, buffer->len, 1);
    }
#else
    if (php_output_get_contents(ret TSRMLS_CC) == FAILURE) {
        php_output_end(TSRMLS_C);
        zend_throw_exception(NULL, "unable to fetch ob content", 0 TSRMLS_CC);
        return FAILURE;
    }
    if (php_output_discard(TSRMLS_C) != SUCCESS) {
        return FAILURE;
    }
#endif
#if ((PHP_MAJOR_VERSION == 5) && (PHP_MINOR_VERSION < 4))

#endif
    return SUCCESS;
}

int linger_view_display(zval *this, zval *tpl, zval *ret TSRMLS_DC)
{
    zval *vars;
    char *script;
    uint len;
    zend_class_entry *old_scope;
    HashTable *scope_vars_table = NULL;

    if (Z_TYPE_P(tpl) != IS_STRING) {
        return FAILURE;
    }

    vars = zend_read_property(view_ce, this, ZEND_STRL(VIEW_PROPERTIES_VARS), 0 TSRMLS_CC);
    if (EG(active_symbol_table)) {
        scope_vars_table = EG(active_symbol_table);
    }

    ALLOC_HASHTABLE(EG(active_symbol_table));
    zend_hash_init(EG(active_symbol_table), 0, NULL, ZVAL_PTR_DTOR, 0);
    (void)linger_view_extract(vars TSRMLS_CC);

    old_scope = EG(scope);
    EG(scope) = view_ce;

    if (IS_ABSOLUTE_PATH(Z_STRVAL_P(tpl), Z_STRLEN_P(tpl))) {
        script = Z_STRVAL_P(tpl);
        len = Z_STRLEN_P(tpl);
        if (linger_application_import(script, len + 1, 0 TSRMLS_CC) == FAILURE) {
            zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "failed opening template %s:%s", script, strerror(errno));
            EG(scope) = old_scope;
            RESTORE_ACTIVE_SYMBOL_TABLE();
            return FAILURE;
        }
    } else {
        zval *tpl_dir = zend_read_property(view_ce, this, ZEND_STRL(VIEW_PROPERTIES_TPLDIR), 0 TSRMLS_CC);
        if (Z_TYPE_P(tpl_dir) != IS_STRING) {
            if (LINGER_FRAMEWORK_G(view_directory)) {
                len = spprintf(&script, 0, "%s%c%s", LINGER_FRAMEWORK_G(view_directory), '/', Z_STRVAL_P(tpl));
            } else {
                zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "could not determine the view path, you should call %s::setScriptPath to specific it", view_ce->name);
                EG(scope) = old_scope;
                RESTORE_ACTIVE_SYMBOL_TABLE();
                return FAILURE;
            }
        } else {
            len = spprintf(&script, 0, "%s%c%s", Z_STRVAL_P(tpl_dir), '/', Z_STRVAL_P(tpl));
        }

        if (linger_application_import(script, len + 1, 0 TSRMLS_CC) == FAILURE) {
            zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "failed opening template %s:%s", script, strerror(errno));
            linger_efree(script);
            EG(scope) = old_scope;
            RESTORE_ACTIVE_SYMBOL_TABLE();
            return FAILURE;
        }
        linger_efree(script);
    }

    EG(scope) = old_scope;
    RESTORE_ACTIVE_SYMBOL_TABLE();
    return SUCCESS;
}

int linger_view_assign(zval *this, zval *key, zval *val TSRMLS_DC)
{
    if (!this || !key || !val) {
        return FAILURE;
    }
    zval *vars = zend_read_property(view_ce, this, ZEND_STRL(VIEW_PROPERTIES_VARS), 1 TSRMLS_CC);
    add_assoc_zval(vars, key, val);
    zend_update_property(view_ce, this, ZEND_STRL(VIEW_PROPERTIES_VARS), vars TSRMLS_CC);
    return SUCCESS;
}

zval *linger_view_getVars(zval *this TSRMLS_DC)
{
    if (!this) {
        return NULL;
    } 
    zval *vars = zend_read_property(view_ce, this, ZEND_STRL(VIEW_PROPERTIES_VARS), 1 TSRMLS_CC);
    return vars;
}

PHP_METHOD(linger_framework_view, __construct)
{

}

PHP_METHOD(linger_framework_view, assign)
{
    char *key = NULL;
    uint key_len;
    zval *val = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sz", &key, &key_len, &val) == FAILURE) {
        return;
    }
    if (key_len > 0 && val != NULL) {
        if (linger_view_assign(getThis(), key, val) == FAILURE) {
            zend_throw_exception(NULL, "assign variable fail");
            return;
        }
        Z_ADDREF_P(val);
    }
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(linger_framework_view, setScriptPath)
{
    zval *path = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &path) == FAILURE) {
        return;
    }
    if (path && Z_TYPE_P(path) == IS_STRING) {
        zend_update_property(view_ce, getThis(), ZEND_STRL(VIEW_PROPERTIES_TPLDIR), path TSRMLS_CC);
    } else {
        zend_throw_exception(NULL, "script path must be a string", 0 TSRMLS_CC);
        return;
    }
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(linger_framework_view, getScriptPath)
{
    zval *path = zend_read_property(view_ce, getThis(), ZEND_STRL(VIEW_PROPERTIES_TPLDIR), 1 TSRMLS_CC);
    RETURN_ZVAL(path, 1, 0);
}

PHP_METHOD(linger_framework_view, display)
{
    zval *tpl = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &tpl) == FAILURE) {
        return;
    }
    if (linger_view_display(getThis(), tpl, return_value TSRMLS_CC) == FAILURE) {
        RETURN_FALSE;
    }
    RETURN_TRUE;
}

PHP_METHOD(linger_framework_view, render)
{
    zval *tpl = NULL;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &tpl) == FAILURE) {
        return;
    }
    if (linger_view_render(getThis(), tpl, return_value TSRMLS_CC) == FAILURE) {
        RETURN_FALSE;
    }
}

PHP_METHOD(linger_framework_view, getVars)
{
    zval *vars = linger_view_getVars(getThis() TSRMLS_CC);
    RETURN_ZVAL(vars, 1, 0);
}

zend_function_entry view_methods[] = {
    PHP_ME(linger_framework_view, __construct, NULL, ZEND_ACC_PROTECTED | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_view, setScriptPath, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_view, getScriptPath, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_view, assign, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_view, display, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_view, render, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_view, getVars, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(view)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\View", view_methods);
    view_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(view_ce, ZEND_STRL(VIEW_PROPERTIES_VARS), ZEND_ACC_PROTECTED);
    zend_declare_property_null(view_ce, ZEND_STRL(VIEW_PROPERTIES_TPLDIR), ZEND_ACC_PROTECTED);
    return SUCCESS;
}
