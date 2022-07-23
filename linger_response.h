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

#ifndef LINGER_RESPONSE_H_
#define LINGER_RESPONSE_H_

#define RESPONSE_PROPERTIES_INSTANCE "_instance"
#define RESPONSE_PROPERTIES_STATUS "_status"
#define RESPONSE_PROPERTIES_HEADER "_header"
#define RESPONSE_PROPERTIES_BODY "_body"

extern zend_class_entry *response_ce;

zval *linger_response_instance(zval *this);
void linger_response_send(zval *this);
int linger_response_set_body(zval *this, zval *body);
int linger_response_set_header(zval *this, zend_string *key, zval *val);
int linger_response_set_status(zval *this, zval *status);

#include "SAPI.h"

#define _404() \
	do { \
		sapi_header_line ctr = { \
			.line = "HTTP/1.1 404 Not Found", \
			.line_len = 22, \
			.response_code = 404}; \
		sapi_header_op(SAPI_HEADER_REPLACE, &ctr); \
	} while (0)

#endif /* LINGER_RESPONSE_H_ */
