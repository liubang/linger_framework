--TEST--
Check for linger_framework presence
--SKIPIF--
<?php if (!extension_loaded("linger_framework")) print "skip"; ?>
--FILE--
<?php 

define('APP_PATH', realpath(__DIR__) . '/');

$app = new linger\framework\Application([
		'app_directory' => APP_PATH . 'app'
]);

$app->init([
	\boot\Router::class
]);

$req = $app->getDispatcher()
	->getRequest()
	->setUri('/index.html')
	->setMethod('GET');

$app->run();

?>
--EXPECT--
ok

