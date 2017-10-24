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

#define PHP_LINGER_FRAMEWORK_VERSION "0.1"

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
LINGER_MINIT_FUNCTION(router);
LINGER_MINIT_FUNCTION(controller);
LINGER_MINIT_FUNCTION(request);
LINGER_MINIT_FUNCTION(response);
LINGER_MINIT_FUNCTION(exception);


#endif	/* PHP_LINGER_FRAMEWORK_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
