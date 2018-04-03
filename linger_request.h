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

#ifndef LINGER_REQUEST_H_
#define LINGER_REQUEST_H_

#define REQUEST_PROPERTIES_INSTANCE     "_instance"
#define REQUEST_PROPERTIES_METHOD       "_method"
#define REQUEST_PROPERTIES_URI          "_uri"
#define REQUEST_PROPERTIES_COOKIE       "_cookie"
#define REQUEST_PROPERTIES_QUERY        "_query"
#define REQUEST_PROPERTIES_PARAM        "_param"
#define REQUEST_PROPERTIES_POST         "_post"
#define REQUEST_PROPERTIES_FILES        "_files"

extern zend_class_entry *request_ce;

zval *linger_request_instance(zval *this);
zval *linger_request_get_request_uri(zval *this);
zval *linger_request_get_request_method(zval *this);
void linger_request_set_param(zval *this, char *key, char *val);
int linger_request_set_params(zval *this, zval *values);
int linger_request_set_post(zval *this, zval *values);

#endif /* LINGER_REQUEST_H_ */
