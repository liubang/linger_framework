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

#ifndef LINGER_CONFIG_H_
#define LINGER_CONFIG_H_

#define CONFIG_PROPERTIES_INSTANCE "_instance"
#define CONFIG_PROPERTIES_CONFIG   "_config"

extern zend_class_entry *config_ce;

zval *linger_config_instance(zval *this, zval *config);

#endif /* LINGER_CONFIG_H_ */
