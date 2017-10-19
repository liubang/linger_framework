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
#include "linger_router_rule.h"

zend_class_entry *linger_bootstrap_ce;

int linger_bootstrap_construct(zend_class_entry *ce, zval *this TSRMLS_CC) {
    if (!instanceof_function(ce, linger_bootstrap_ce)) {
        zend_throw_exception_ex(NULL, 0 TSRMLS_CC, "class %s must be subclass of %s", ce->name, linger_bootstrap_ce->name);
        return FAILURE;
    }
    
}

zend_function_entry linger_bootstrap_methods[] = {
    PHP_ABSTRACT_ME(linger_framework_bootstrap, bootstrap, NULL)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(bootstrap)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\Bootstrap", linger_bootstrap_methods);
    linger_bootstrap_ce = zend_register_internal_interface(&ce TSRMLS_CC);

    return SUCCESS;
}
