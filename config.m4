PHP_ARG_ENABLE(linger_framework, whether to enable linger_framework support,
[  --enable-linger_framework           Enable linger_framework support])

if test "$PHP_LINGER_FRAMEWORK" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-linger_framework -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/linger_framework.h"  # you most likely want to change this
  dnl if test -r $PHP_LINGER_FRAMEWORK/$SEARCH_FOR; then # path given as parameter
  dnl   LINGER_FRAMEWORK_DIR=$PHP_LINGER_FRAMEWORK
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for linger_framework files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       LINGER_FRAMEWORK_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$LINGER_FRAMEWORK_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the linger_framework distribution])
  dnl fi

  dnl # --with-linger_framework -> add include path
  dnl PHP_ADD_INCLUDE($LINGER_FRAMEWORK_DIR/include)

  dnl # --with-linger_framework -> check for lib and symbol presence
  dnl LIBNAME=linger_framework # you may want to change this
  dnl LIBSYMBOL=linger_framework # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $LINGER_FRAMEWORK_DIR/$PHP_LIBDIR, LINGER_FRAMEWORK_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_LINGER_FRAMEWORKLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong linger_framework lib version or lib not found])
  dnl ],[
  dnl   -L$LINGER_FRAMEWORK_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  PHP_SUBST(LINGER_FRAMEWORK_SHARED_LIBADD)

  PHP_NEW_EXTENSION(linger_framework, 
    linger_framework.c      \
    linger_application.c    \
    linger_config.c         \
    linger_router.c         \
    linger_dispatcher.c     \
    linger_controller.c     \
    linger_request.c        \
    linger_response.c       \
    linger_exception.c, 
    $ext_shared)
fi
