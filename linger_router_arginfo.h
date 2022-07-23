/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: d6c5e554950fec84e0a8ac551027a7870b203c50 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_Linger_Framework_Router___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Linger_Framework_Router_add, 0, 1, IS_OBJECT, 0)
	ZEND_ARG_TYPE_INFO(0, rule, IS_OBJECT, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Linger_Framework_Router_get, 0, 3, IS_OBJECT, 0)
	ZEND_ARG_TYPE_INFO(0, uri, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, clazz, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, method, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_Linger_Framework_Router_put arginfo_class_Linger_Framework_Router_get

#define arginfo_class_Linger_Framework_Router_post arginfo_class_Linger_Framework_Router_get

#define arginfo_class_Linger_Framework_Router_delete arginfo_class_Linger_Framework_Router_get

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Linger_Framework_Router_setChunkSize, 0, 1, IS_OBJECT, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Linger_Framework_Router_getRules, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_Linger_Framework_Router_dump, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()
