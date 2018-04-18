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

#ifndef LINGER_ROUTER_H_
#define LINGER_ROUTER_H_

#define LINGER_ROUTER_PROPERTIES_INSTANCE        "_instance"
#define LINGER_ROUTER_PROPERTIES_RULES           "_rules"
#define LINGER_ROUTER_PROPERTIES_MAX_INDEX       "_max_index"
#define LINGER_ROUTER_PROPERTIES_CURR_CHUNK      "_curr_chunk"
#define LINGER_ROUTER_PROPERTIES_CURR_NUM        "_curr_num"
#define LINGER_ROUTER_PROPERTIES_CHUNK_SIZE      "_chunk_size"

extern zend_class_entry *router_ce;

zval *linger_router_instance(zval *this);
zval *linger_router_match(zval *this, zval *request);
zval *linger_router_match_ex(zval *this, zval *request);

#endif /* LINGER_ROUTER_H_ */
