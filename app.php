<?php

error_reporting(E_ALL);

try {

    define('APP_PATH', realpath(__DIR__) . '/');
    $app = new linger\framework\Application([
        'app_directory' => APP_PATH . 'app'
	]);

	//var_dump($app);
	//$app->getRequest()->setUri('/index/index/index');
	$app->getRequest()->setUri('/index/index/index/aaa/bbb');
	$app->run();
} catch (Exception $e) {
	echo $e->getMessage(), PHP_EOL;
	echo $e->getTraceAsString(), PHP_EOL;
}
