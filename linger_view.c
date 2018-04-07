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
#include "linger_view.h"
#include "php_linger_framework.h"

zend_class_entry *view_ce;

ZEND_BEGIN_ARG_INFO_EX(linger_framework_view_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(linger_framework_view_1_arginfo, 0, 0, 1)
ZEND_ARG_INFO(0, tpl)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(linger_framework_view_2_arginfo, 0, 0, 2)
ZEND_ARG_INFO(0, key)
ZEND_ARG_INFO(0, val)
ZEND_END_ARG_INFO()

void linger_view_instance(zval *this)
{
    if (Z_ISUNDEF_P(this)) {
        object_init_ex(this, view_ce);
        zval vars = {{0}}, tpl_dir = {{0}};
        array_init(&vars);
        zend_update_property(view_ce, this, ZEND_STRL(VIEW_PROPERTIES_VARS), &vars);
        zend_update_property(view_ce, this, ZEND_STRL(VIEW_PROPERTIES_TPLDIR), &tpl_dir);
        zval_ptr_dtor(&vars);
    }
}


PHP_METHOD(linger_framework_view, setScriptPath)
{
    zval *tpl;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &tpl) == FAILURE) {
        return;
    }

    if (IS_STRING != Z_TYPE_P(tpl)) {
        linger_throw_exception(NULL, 0, "the parameter must be string.");
        return;
    }

    zend_update_property(view_ce, getThis(), ZEND_STRL(VIEW_PROPERTIES_TPLDIR), tpl);

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(linger_framework_view, getScriptPath)
{
    zval *tpl = zend_read_property(view_ce, getThis(), ZEND_STRL(VIEW_PROPERTIES_TPLDIR), 1, NULL);

    RETURN_ZVAL(tpl, 1, 0);
}

PHP_METHOD(linger_framework_view, display)
{
    zval *tpl;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &tpl) == FAILURE) {
        return;
    }

    if (IS_STRING != Z_TYPE_P(tpl)) {
        linger_throw_exception(NULL, 0, "the parameter must be string.");
        return;
    }

    zval *vars = zend_read_property(view_ce, getThis(), ZEND_STRL(VIEW_PROPERTIES_VARS), 1, NULL);

    char *script = NULL;
    int script_len = 0;

    if (IS_ABSOLUTE_PATH(Z_STRVAL_P(tpl), Z_STRLEN_P(tpl))) {
        script = Z_STRVAL_P(tpl);
        script_len = Z_STRLEN_P(tpl);
    } else {
        zval *tpl_dir = zend_read_property(view_ce, getThis(), ZEND_STRL(VIEW_PROPERTIES_TPLDIR), 1, NULL);
        if (!tpl_dir || Z_TYPE_P(tpl_dir) != IS_STRING) {
            if (LINGER_FRAMEWORK_G(view_directory)) {
                script_len = spprintf(&script, 0, "%s%c%s", LINGER_FRAMEWORK_G(view_directory), '/', Z_STRVAL_P(tpl));
            } else {
                linger_throw_exception(NULL, 0, "could not determine the view path.");
                return;
            }
        } else {
            script_len = spprintf(&script, 0, "%s%c%s", Z_STRVAL_P(tpl_dir), '/', Z_STRVAL_P(tpl));
        }
    }

    zend_array *symbol_table = zend_rebuild_symbol_table();
    extract_ref_overwrite(Z_ARRVAL_P(vars), symbol_table);

    zval retval = {{0}};
    linger_framework_include_scripts(script, script_len, &retval);
    zval_ptr_dtor(&retval);
    linger_efree(script);

    RETURN_TRUE;
}

PHP_METHOD(linger_framework_view, render)
{
    zval *tpl;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "z", &tpl) == FAILURE) {
        return;
    }

    if (IS_STRING != Z_TYPE_P(tpl)) {
        linger_throw_exception(NULL, 0, "the parameter must be string.");
        return;
    }

    zval *vars = zend_read_property(view_ce, getThis(), ZEND_STRL(VIEW_PROPERTIES_VARS), 1, NULL);

    char *script = NULL;
    int script_len = 0;

    if (IS_ABSOLUTE_PATH(Z_STRVAL_P(tpl), Z_STRLEN_P(tpl))) {
        script = Z_STRVAL_P(tpl);
        script_len = Z_STRLEN_P(tpl);
    } else {
        zval *tpl_dir = zend_read_property(view_ce, getThis(), ZEND_STRL(VIEW_PROPERTIES_TPLDIR), 1, NULL);
        if (!tpl_dir || Z_TYPE_P(tpl_dir) != IS_STRING) {
            if (LINGER_FRAMEWORK_G(view_directory)) {
                script_len = spprintf(&script, 0, "%s%c%s", LINGER_FRAMEWORK_G(view_directory), '/', Z_STRVAL_P(tpl));
            } else {
                linger_throw_exception(NULL, 0, "could not determine the view path.");
                return;
            }
        } else {
            script_len = spprintf(&script, 0, "%s%c%s", Z_STRVAL_P(tpl_dir), '/', Z_STRVAL_P(tpl));
        }
    }

    zend_array *symbol_table = zend_rebuild_symbol_table();
    extract_ref_overwrite(Z_ARRVAL_P(vars), symbol_table);

    zval retval = {{0}};
    php_output_start_user(NULL, 0, PHP_OUTPUT_HANDLER_STDFLAGS);
    linger_framework_include_scripts(script, script_len, &retval);
    zval_ptr_dtor(&retval);
    linger_efree(script);
    php_output_get_contents(return_value);

    return;
}

PHP_METHOD(linger_framework_view, getVars)
{
    zval *vars = zend_read_property(view_ce, getThis(), ZEND_STRL(VIEW_PROPERTIES_VARS), 1, NULL);

    RETURN_ZVAL(vars, 1, 0);
}

PHP_METHOD(linger_framework_view, assign)
{
    zend_string *key = NULL;
    zval *val = NULL;
    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "Sz", &key, &val) == FAILURE) {
        return;
    }

    if (!key || !val) {
        linger_throw_exception(NULL, 0, "parameter error.");
        return;
    }

    zval *vars = zend_read_property(view_ce, getThis(), ZEND_STRL(VIEW_PROPERTIES_VARS), 1, NULL);
    add_assoc_zval(vars, ZSTR_VAL(key), val);
    Z_TRY_ADDREF_P(val);

    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(linger_framework_view, __construct)
{

}

zend_function_entry view_methods[] = {
    PHP_ME(linger_framework_view, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_view, setScriptPath, linger_framework_view_1_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_view, getScriptPath, linger_framework_view_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_view, display, linger_framework_view_1_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_view, render, linger_framework_view_1_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_view, getVars, linger_framework_view_void_arginfo, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_view, assign, linger_framework_view_2_arginfo, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(view)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\View", view_methods);
    view_ce = zend_register_internal_class_ex(&ce, NULL);
    zend_declare_property_null(view_ce, ZEND_STRL(VIEW_PROPERTIES_VARS), ZEND_ACC_PROTECTED);
    zend_declare_property_null(view_ce, ZEND_STRL(VIEW_PROPERTIES_TPLDIR), ZEND_ACC_PROTECTED);
    return SUCCESS;
}
