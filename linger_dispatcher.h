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

#ifndef LINGER_DISPATCHER_H_
#define LINGER_DISPATCHER_H_

#define DISPATCHER_PROPERTIES_INSTANCE       "_instance"
#define DISPATCHER_PROPERTIES_REQUEST        "_request"
#define DISPATCHER_PROPERTIES_ROUTER         "_router"
#define DISPATCHER_PROPERTIES_MODULE         "_module"
#define DISPATCHER_PROPERTIES_CONTROLLER     "_controller"
#define DISPATCHER_PROPERTIES_ACTION         "_action"

zval *linger_dispatcher_instance(zval *this, zval *orequest, zval *router);
void linger_dispatcher_prepare(zval *this);
void linger_dispatcher_dispatch(zval *this);

#endif /* LINGER_DISPATCHER_H_ */
