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
	//$rule = new \linger\framework\RouterRule('GET', '/test/aaa', 'Test', 'bootstrap');
    //\var_dump($rule);die;
    //$router = new \linger\framework\Router();
    $router = $app->getRouter();

    //$router->add($rule);
    $router->get('/get/@userId:([0-9]+)/@orderId:([0-9]+)', 'Test', 'bootstrap');
//            ->post('/post', 'Test', 'bootstrap')
 //           ->put('/put', 'Test', 'bootstrap')
  //          ->delete('/delete', 'Test', 'bootstrap');
   var_dump($router);

	//var_dump($app);
	$app->getRequest()->setMethod('get')->setUri('/get/124/2345');
    var_dump($app->getDispatcher()->findRouter());
	//$app->run();
} catch (Exception $e) {
	echo $e->getMessage(), PHP_EOL;
	echo $e->getTraceAsString(), PHP_EOL;
}
