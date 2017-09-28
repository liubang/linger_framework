<?php

error_reporting(E_ALL);


try {

	//include __DIR__ . '/app/module/index/controller/Index.php';

	$app = new linger\framework\Application([
    		'name' => 'test'
	]);

	//var_dump($app);
	//$app->getRequest()->setUri('/index/index/index');
	$app->getRequest()->setUri('/index/index/index/aaa/bbb');
	$app->run();
} catch (Exception $e) {
	echo $e->getMessage(), PHP_EOL;
	echo $e->getTraceAsString(), PHP_EOL;
}
