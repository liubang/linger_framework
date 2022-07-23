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

#ifndef LINGER_APPLICATION_H_
#define LINGER_APPLICATION_H_

#ifndef MAXPATHLEN
# define MAXPATHLEN                        1024
#endif /* ifndef SYMBOL */
#define APPLICATION_PROPERTIES_APP        "_app"
#define APPLICATION_PROPERTIES_CONFIG     "_config"
#define APPLICATION_PROPERTIES_ROUTER     "_router"
#define APPLICATION_PROPERTIES_RESPONSE   "_response"
#define APPLICATION_PROPERTIES_DISPATCHER "_dispatcher"

extern zend_class_entry *application_ce;

#endif /* LINGER_APPLICATION_H_ */
