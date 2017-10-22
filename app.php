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
	$rule = new \linger\framework\RouterRule('GET', '/test/aaa', 'Test', 'bootstrap');
    //\var_dump($rule);die;
    //$router = new \linger\framework\Router();
    $router = $app->getRouter();
    var_dump($router);

    $router->add($rule);
    $router->get('/get', 'Test', 'bootstrap')
            ->post('/post', 'Test', 'bootstrap')
            ->put('/put', 'Test', 'bootstrap')
            ->delete('/delete', 'Test', 'bootstrap');
   var_dump($router);

	//var_dump($app);
	$app->getRequest()->setUri('/index/index/index');
	//$app->run();
} catch (Exception $e) {
	echo $e->getMessage(), PHP_EOL;
	echo $e->getTraceAsString(), PHP_EOL;
}
