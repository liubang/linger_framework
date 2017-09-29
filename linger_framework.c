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

ZEND_DECLARE_MODULE_GLOBALS(linger_framework)

static int le_linger_framework;

PHP_INI_BEGIN()
STD_PHP_INI_ENTRY("linger_framework.display_errors", "1", PHP_INI_ALL, OnUpdateBool, display_errors, zend_linger_framework_globals, linger_framework_globals)
STD_PHP_INI_ENTRY("linger_framework.throw_exception", "1", PHP_INI_ALL, OnUpdateBool, throw_exception, zend_linger_framework_globals, linger_framework_globals)
PHP_INI_END()

static void php_linger_framework_init_globals(zend_linger_framework_globals *linger_framework_globals)
{
    linger_framework_globals->display_errors = 1;
    linger_framework_globals->throw_exception = 1;
	linger_framework_globals->view_directory = NULL;
	linger_framework_globals->app_directory = NULL;
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(linger_framework)
{
	ZEND_INIT_MODULE_GLOBALS(linger_framework, php_linger_framework_init_globals, NULL);
    REGISTER_INI_ENTRIES();

    LINGER_STARTUP(application);
    LINGER_STARTUP(config);
    LINGER_STARTUP(router);
    LINGER_STARTUP(request);
    LINGER_STARTUP(dispatcher);
    LINGER_STARTUP(controller);
    LINGER_STARTUP(view);
    //LINGER_STARTUP(response);
    //LINGER_STARTUP(exception);

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(linger_framework)
{
    UNREGISTER_INI_ENTRIES();
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(linger_framework)
{
	LINGER_FRAMEWORK_G(app_directory) = NULL;
	LINGER_FRAMEWORK_G(view_directory) = NULL;
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(linger_framework)
{
	linger_efree(LINGER_FRAMEWORK_G(app_directory));
	linger_efree(LINGER_FRAMEWORK_G(view_directory));
	
    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(linger_framework)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "linger_framework support", "enabled");
    php_info_print_table_end();
    DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ linger_framework_module_entry
 */
zend_module_entry linger_framework_module_entry = {
    STANDARD_MODULE_HEADER,
    "linger_framework",
    NULL,
    PHP_MINIT(linger_framework),
    PHP_MSHUTDOWN(linger_framework),
    PHP_RINIT(linger_framework),
    PHP_RSHUTDOWN(linger_framework),
    PHP_MINFO(linger_framework),
    PHP_LINGER_FRAMEWORK_VERSION,
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_LINGER_FRAMEWORK
ZEND_GET_MODULE(linger_framework)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
