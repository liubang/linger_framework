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

/* $Id$ */

#ifndef PHP_LINGER_FRAMEWORK_H
#define PHP_LINGER_FRAMEWORK_H

extern zend_module_entry linger_framework_module_entry;
extern zend_class_entry *config_ce;
extern zend_class_entry *request_ce;
extern zend_class_entry *router_rule_ce;
extern zend_class_entry *bootstrap_ce;
#define phpext_linger_framework_ptr &linger_framework_module_entry

#define PHP_LINGER_FRAMEWORK_VERSION "2.1-dev"

#ifdef PHP_WIN32
#	define PHP_LINGER_FRAMEWORK_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_LINGER_FRAMEWORK_API __attribute__ ((visibility("default")))
#else
#	define PHP_LINGER_FRAMEWORK_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif


ZEND_BEGIN_MODULE_GLOBALS(linger_framework)
zend_bool display_errors;
zend_bool throw_exception;
char *view_directory;
char *app_directory;
ZEND_END_MODULE_GLOBALS(linger_framework)

extern ZEND_DECLARE_MODULE_GLOBALS(linger_framework);

#ifdef ZTS
#define LINGER_FRAMEWORK_G(v) TSRMG(linger_framework_globals_id, zend_linger_framework_globals *, v)
#else
#define LINGER_FRAMEWORK_G(v) (linger_framework_globals.v)
#endif

#define LINGER_MINIT_FUNCTION(module) ZEND_MINIT_FUNCTION(linger_framework_##module)
#define LINGER_STARTUP(module)		  ZEND_MODULE_STARTUP_N(linger_framework_##module)(INIT_FUNC_ARGS_PASSTHRU)

#define linger_efree(ptr)	if (ptr) efree(ptr)
#define linger_php_error(level, fmt_str, ...)	if (LINGER_FRAMEWORK_G(display_errors)) php_error_docref(NULL TSRMLS_CC, level, fmt_str, ##__VA_ARGS__)
#define linger_php_fatal_error(level, fmt_str, ...)  php_error_docref(NULL TSRMLS_CC, level, fmt_str, ##__VA_ARGS__)
#define linger_throw_exception(e, level, fmt_str, ...) zend_throw_exception_ex(e, level TSRMLS_CC, fmt_str, ##__VA_ARGS__)

/* declare components */
LINGER_MINIT_FUNCTION(application);
LINGER_MINIT_FUNCTION(config);
LINGER_MINIT_FUNCTION(bootstrap);
LINGER_MINIT_FUNCTION(router);
LINGER_MINIT_FUNCTION(router_rule);
LINGER_MINIT_FUNCTION(request);
LINGER_MINIT_FUNCTION(dispatcher);
LINGER_MINIT_FUNCTION(controller);
LINGER_MINIT_FUNCTION(view);
LINGER_MINIT_FUNCTION(response);
LINGER_MINIT_FUNCTION(exception);

#include "Zend/zend_exceptions.h"

static int zend_always_inline linger_framework_include_scripts(char *file, int len, zval *retval) /* {{{ */
{
    zend_file_handle file_handle;
    zend_op_array *op_array;

    file_handle.filename = file;
    file_handle.free_filename = 0;
    file_handle.type = ZEND_HANDLE_FILENAME;
    file_handle.opened_path = NULL;
    file_handle.handle.fp = NULL;

    op_array = zend_compile_file(&file_handle, ZEND_REQUIRE);

    if (file_handle.opened_path) {
        zend_hash_add_empty_element(&EG(included_files), file_handle.opened_path);
    }
    zend_destroy_file_handle(&file_handle);

    if (op_array) {
        zend_execute(op_array, retval);
        zend_exception_restore();
        zend_try_exception_handler();
        if (EG(exception)) {
            zend_exception_error(EG(exception), E_ERROR);
        }
        destroy_op_array(op_array);
        efree_size(op_array, sizeof(zend_op_array));

        return SUCCESS;
    }

    return FAILURE;
}
/* }}} */

static zend_always_inline int php_valid_var_name(char *var_name, size_t var_name_len) /* {{{ */
{
#if 1
    /* first 256 bits for first character, and second 256 bits for the next */
    static const uint32_t charset[16] = {
        /*  31      0   63     32   95     64   127    96 */
        0x00000000, 0x00000000, 0x87fffffe, 0x07fffffe,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
        /*  31      0   63     32   95     64   127    96 */
        0x00000000, 0x03ff0000, 0x87fffffe, 0x07fffffe,
        0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff
    };
#endif
    size_t i;
    uint32_t ch;

    if (UNEXPECTED(!var_name_len)) {
        return 0;
    }

    /* These are allowed as first char: [a-zA-Z_\x7f-\xff] */
    ch = (uint32_t)((unsigned char *)var_name)[0];
#if 1
    if (UNEXPECTED(!(charset[ch >> 5] & (1 << (ch & 0x1f))))) {
#else
    if (var_name[0] != '_' &&
            (ch < 65  /* A    */ || /* Z    */ ch > 90)  &&
            (ch < 97  /* a    */ || /* z    */ ch > 122) &&
            (ch < 127 /* 0x7f */ || /* 0xff */ ch > 255)
       ) {
#endif
        return 0;
    }

    /* And these as the rest: [a-zA-Z0-9_\x7f-\xff] */
    if (var_name_len > 1) {
        i = 1;
        do {
            ch = (uint32_t)((unsigned char *)var_name)[i];
#if 1
            if (UNEXPECTED(!(charset[8 + (ch >> 5)] & (1 << (ch & 0x1f))))) {
#else
            if (var_name[i] != '_' &&
                    (ch < 48  /* 0    */ || /* 9    */ ch > 57)  &&
                    (ch < 65  /* A    */ || /* Z    */ ch > 90)  &&
                    (ch < 97  /* a    */ || /* z    */ ch > 122) &&
                    (ch < 127 /* 0x7f */ || /* 0xff */ ch > 255)
               ) {
#endif
                return 0;
            }
        }
        while (++i < var_name_len);
    }
    return 1;
}
/* }}} */

static zend_long zend_always_inline extract_ref_overwrite(zend_array *arr, zend_array *symbol_table) /* {{{ */
{
    int exception_thrown = 0;
    zend_long count = 0;
    zend_string *var_name;
    zval *entry, *orig_var;

    ZEND_HASH_FOREACH_STR_KEY_VAL_IND(arr, var_name, entry) {
        if (!var_name) {
            continue;
        }
        if (!php_valid_var_name(ZSTR_VAL(var_name), ZSTR_LEN(var_name))) {
            continue;
        }
        if (ZSTR_LEN(var_name) == sizeof("this")-1  && !strcmp(ZSTR_VAL(var_name), "this")) {
            if (!exception_thrown) {
                exception_thrown = 1;
                zend_throw_error(NULL, "Cannot re-assign $this");
            }
            continue;
        }
        orig_var = zend_hash_find(symbol_table, var_name);
        if (orig_var) {
            if (Z_TYPE_P(orig_var) == IS_INDIRECT) {
                orig_var = Z_INDIRECT_P(orig_var);
            }
            if (ZSTR_LEN(var_name) == sizeof("GLOBALS")-1 && !strcmp(ZSTR_VAL(var_name), "GLOBALS")) {
                continue;
            }
            ZVAL_DEREF(entry);
            if (Z_REFCOUNTED_P(entry)) Z_ADDREF_P(entry);
            ZVAL_DEREF(orig_var);
            zval_ptr_dtor(orig_var);
            ZVAL_COPY_VALUE(orig_var, entry);
        } else {
            ZVAL_DEREF(entry);
            if (Z_REFCOUNTED_P(entry)) Z_ADDREF_P(entry);
            zend_hash_add_new(symbol_table, var_name, entry);
        }
        count++;
    }
    ZEND_HASH_FOREACH_END();

    return count;
}
/* }}} */

#endif	/* PHP_LINGER_FRAMEWORK_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
