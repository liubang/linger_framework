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
#include "linger_config.h"
#include "linger_router.h"
#include "linger_request.h"
#include "linger_dispatcher.h"
#include "linger_response.h"

zend_class_entry *application_ce;
zend_class_entry *config_ce;

#define APPLICATION_PROPERTIES_APP        "_app"
#define APPLICATION_PROPERTIES_CONFIG     "_config"
#define APPLICATION_PROPERTIES_ROUTER     "_router"
#define APPLICATION_PROPERTIES_REQUEST    "_request"
#define APPLICATION_PROPERTIES_RESPONSE   "_response"
#define APPLICATION_PROPERTIES_DISPATCHER "_dispatcher"

PHP_METHOD(linger_framework_application, __construct)
{
    zval *oconfig, *aconfig;
    zval *orouter;
    zval *orequest;
    zval *oresponse;
    zval *odispatcher;
    zval *app;
    
    app = zend_read_static_property(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_APP), 1 TSRMLS_CC);
    
    if (!ZVAL_IS_NULL(app)) {
        zend_throw_exception(NULL, "Can not reinstance application", 0 TSRMLS_CC);        
        RETURN_FALSE;
    }
    
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &aconfig) == FAILURE) {
        return;
    }
    zval *self = getThis();
    oconfig = linger_config_instance(NULL, aconfig TSRMLS_CC);
    zend_update_property(application_ce, self, ZEND_STRL(APPLICATION_PROPERTIES_CONFIG), oconfig TSRMLS_CC);
   
    orequest = linger_request_instance(NULL, NULL TSRMLS_CC);
    odispatcher = linger_dispatcher_instance(orequest TSRMLS_CC);
    zend_update_property(application_ce, self, ZEND_STRL(APPLICATION_PROPERTIES_DISPATCHER), odispatcher TSRMLS_CC);
    zend_update_property(application_ce, self, ZEND_STRL(APPLICATION_PROPERTIES_REQUEST), orequest TSRMLS_CC);
    zend_update_static_property(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_APP), self TSRMLS_CC);
    zval_ptr_dtor(&oconfig);
    zval_ptr_dtor(&orequest);
    zval_ptr_dtor(&odispatcher);
}

PHP_METHOD(linger_framework_application, run)
{
    //dispatcher dispatche. 
}

PHP_METHOD(linger_framework_application, app)
{
    //static method to get app instance.
    zval *app = zend_read_static_property(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_APP), 1 TSRMLS_CC);      
    RETURN_ZVAL(app, 1, 0);
}

PHP_METHOD(linger_framework_application, getConfig)
{
    zval *config = zend_read_property(application_ce, getThis(), ZEND_STRL(APPLICATION_PROPERTIES_CONFIG), 1 TSRMLS_CC);
    RETURN_ZVAL(config, 1, 0);
}

PHP_METHOD(linger_framework_application, getDispatcher)
{
    zval *dispatcher = zend_read_property(application_ce, getThis(), ZEND_STRL(APPLICATION_PROPERTIES_DISPATCHER), 1 TSRMLS_CC);
    RETURN_ZVAL(dispatcher, 1, 0);
}

PHP_METHOD(linger_framework_application, getRequest)
{
    zval *request = zend_read_property(application_ce, getThis(), ZEND_STRL(APPLICATION_PROPERTIES_REQUEST), 1 TSRMLS_CC);
    RETURN_ZVAL(request, 1, 0);
}

PHP_METHOD(linger_framework_application, setConfig)
{
    zval *config;
    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &config) == FAILURE) {
        return;
    }
    if (Z_TYPE_P(config) == IS_OBJECT &&
            instanceof_function(Z_OBJCE_P(config), config_ce)) {
        zend_update_property(application_ce, getThis(), ZEND_STRL(APPLICATION_PROPERTIES_CONFIG), config TSRMLS_CC);
        RETURN_ZVAL(getThis(), 1, 0);
    } else {
        zend_throw_exception(NULL, "config must be a instance of linger_framework_Config");
    }
}

PHP_METHOD(linger_framework_application, __destruct)
{
    zend_update_static_property_null(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_APP));
}

zend_function_entry application_methods[] = {
    PHP_ME(linger_framework_application, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR) 
    PHP_ME(linger_framework_application, run, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_application, app, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_ME(linger_framework_application, getConfig, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_application, setConfig, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_application, getDispatcher, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_application, getRequest, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(linger_framework_application, __destruct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_DTOR)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(application)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\Framework\\Application", application_methods);
    application_ce = zend_register_internal_class(&ce TSRMLS_CC);
    zend_declare_property_null(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_APP), ZEND_ACC_PROTECTED | ZEND_ACC_STATIC);
    zend_declare_property_null(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_CONFIG), ZEND_ACC_PROTECTED);
    zend_declare_property_null(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_REQUEST), ZEND_ACC_PROTECTED);
    zend_declare_property_null(application_ce, ZEND_STRL(APPLICATION_PROPERTIES_DISPATCHER), ZEND_ACC_PROTECTED);
    return SUCCESS;
}
