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

#ifndef LINGER_ROUTER_RULE_H_
#define LINGER_ROUTER_RULE_H_

#define ROUTER_RULE_PROPERTIES_REQUEST_METHOD "_request_method"
#define ROUTER_RULE_PROPERTIES_URI            "_uri"
#define ROUTER_RULE_PROPERTIES_COMPILED_URI   "_compiled_uri"
#define ROUTER_RULE_PROPERTIES_PARAMS_MAP     "_params_map"
#define ROUTER_RULE_PROPERTIES_CLASS          "_class"
#define ROUTER_RULE_PROPERTIES_CLASS_METHOD   "_class_method"

extern zend_class_entry *router_rule_ce;

typedef struct _router_rule_obj {
    zend_string *zs_uri;
    zend_object std;
} router_rule_obj;

#define Z_GET_ROUTER_RULE_OBJ(zv) \
    (router_rule_obj *)((char *)Z_OBJ_P((zv)) - XtOffsetOf(router_rule_obj, std))

#define ROUTER_RULE_OBJ_GET_ZS_URI(obj) \
    (obj)->zs_uri

#define Z_GET_ZS_URI(zv) \
    ROUTER_RULE_OBJ_GET_ZS_URI(Z_GET_ROUTER_RULE_OBJ(zv))


zval *linger_router_rule_instance(zval *this, zval *request_method, zval *uri, zval *class, zval *class_method);
zval *linger_router_rule_get_request_method(zval *this);
zval *linger_router_rule_get_uri(zval *this);
zval *linger_router_rule_get_class(zval *this);
zval *linger_router_rule_get_class_method(zval *this);
int linger_router_rule_set_compiled_uri(zval *this, zend_string *compiled_uri);
zend_string *linger_router_rule_get_compiled_uri(zval *this);
int linger_router_rule_set_params_map(zval *this, zval *params_map);
zval *linger_router_rule_get_params_map(zval *this);

#endif /* LINGER_ROUTER_RULE_H_ */
