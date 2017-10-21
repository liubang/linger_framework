<?php

error_reporting(E_ALL);
define('APP_PATH', realpath(__DIR__) . '/');

require APP_PATH . 'app/boot/A.php';


$bootclass = [
    \boot\A::class
];

print_r($bootclass);
try {
    $app = new linger\framework\Application([
        'app_directory' => APP_PATH . 'app'
	]);

    $app->init($bootclass);

	//var_dump($app);
	//$app->getRequest()->setUri('/index/index/index');
	$app->getRequest()->setUri('/index/index/index/aaa/bbb');
	$app->run();
} catch (Exception $e) {
	echo $e->getMessage(), PHP_EOL;
	echo $e->getTraceAsString(), PHP_EOL;
}
