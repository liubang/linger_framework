<?php

error_reporting(E_ALL);
define('APP_PATH', realpath(__DIR__) . '/');
require APP_PATH . 'app/boot/Test.php';

$bootclass = [
    \boot\Test::class
];

print_r($bootclass);

try {
    $app = new linger\framework\Application([
        'app_directory' => APP_PATH . 'app'
	]);
    $app->init($bootclass);
    $router = $app->getRouter();
    $router->get('/get/@userId:([0-9]+)/@orderId:([0-9]+)', 'Test', 'bootstrap');
	$app->getRequest()->setMethod('get')->setUri('/get/124/2345');
    var_dump($app->getDispatcher()->findRouter());
    die;
} catch (Exception $e) {
	echo $e->getMessage(), PHP_EOL;
	echo $e->getTraceAsString(), PHP_EOL;
}
