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
#include "Zend/zend_interfaces.h"
#include "php_linger_framework.h"
#include "linger_request.h"
#include "linger_response.h"
#include "linger_controller.h"
#include "linger_view.h"

zend_class_entry *controller_ce;

ZEND_BEGIN_ARG_INFO_EX(linger_framework_controller_void_arginfo, 0, 0, 0)
ZEND_END_ARG_INFO()

int linger_controller_construct(zend_class_entry *ce, zval *this, zval *request)
{
    if (UNEXPECTED(!instanceof_function(ce, controller_ce))) {
        linger_throw_exception(NULL, 0, "controller must be a subclass of linger\\framework\\Controller.");
        return FAILURE;
    }

    if (UNEXPECTED(!instanceof_function(Z_OBJCE_P(request), request_ce))) {
        linger_throw_exception(NULL, 0, "request must be a instance of linger\\framework\\Request.");
        return FAILURE;
    }

    zend_update_property(controller_ce, this, ZEND_STRL(CONTROLLER_PROPERTIES_REQUEST), request);

    zval response = {{0}};
    (void)linger_response_instance(&response);
    zend_update_property(controller_ce, this, ZEND_STRL(CONTROLLER_PROPERTIES_RESPONSE), &response);
    zval_ptr_dtor(&response);

    zval view = {{0}};
    linger_view_instance(&view);
    zend_update_property(controller_ce, this, ZEND_STRL(CONTROLLER_PROPERTIES_VIEW), &view);
    zval_ptr_dtor(&view);

    zend_string *init = zend_string_init("_init", sizeof("_init") - 1, 0);
    if (zend_hash_exists(&(ce->function_table), init)) {
        zend_call_method_with_0_params(this, ce, NULL, "_init", NULL);
    }
    zend_string_release(init);

    return SUCCESS;
}

PHP_METHOD(linger_framework_controller, __construct)
{

}

PHP_METHOD(linger_framework_controller, _init)
{

}

PHP_METHOD(linger_framework_controller, getRequest)
{
    zval *request = zend_read_property(controller_ce, getThis(), ZEND_STRL(CONTROLLER_PROPERTIES_REQUEST), 1, NULL);
    RETURN_ZVAL(request, 1, 0);
}

PHP_METHOD(linger_framework_controller, getResponse)
{
    zval *response= zend_read_property(controller_ce, getThis(), ZEND_STRL(CONTROLLER_PROPERTIES_RESPONSE), 1, NULL);
    RETURN_ZVAL(response, 1, 0);
}

PHP_METHOD(linger_framework_controller, getView)
{
    zval *view = zend_read_property(controller_ce, getThis(), ZEND_STRL(CONTROLLER_PROPERTIES_VIEW), 1, NULL);
    RETURN_ZVAL(view, 1, 0);
}

zend_function_entry controller_methods[] = {
    PHP_ME(linger_framework_controller, __construct, NULL, ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
    PHP_ME(linger_framework_controller, _init, linger_framework_controller_void_arginfo, ZEND_ACC_PROTECTED)
    PHP_ME(linger_framework_controller, getRequest, linger_framework_controller_void_arginfo, ZEND_ACC_PROTECTED)
    PHP_ME(linger_framework_controller, getResponse, linger_framework_controller_void_arginfo, ZEND_ACC_PROTECTED)
    PHP_ME(linger_framework_controller, getView, linger_framework_controller_void_arginfo, ZEND_ACC_PROTECTED)
    PHP_FE_END
};

LINGER_MINIT_FUNCTION(controller)
{
    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Linger\\framework\\Controller", controller_methods);
    controller_ce = zend_register_internal_class_ex(&ce, NULL);
    controller_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

    zend_declare_property_null(controller_ce, ZEND_STRL(CONTROLLER_PROPERTIES_REQUEST), ZEND_ACC_PROTECTED);
    zend_declare_property_null(controller_ce, ZEND_STRL(CONTROLLER_PROPERTIES_RESPONSE), ZEND_ACC_PROTECTED);
    zend_declare_property_null(controller_ce, ZEND_STRL(CONTROLLER_PROPERTIES_VIEW), ZEND_ACC_PROTECTED);
    return SUCCESS;
}
