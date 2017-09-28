<?php

include __DIR__ . '/app/module/index/controller/Index.php';

$app = new linger\framework\Application([
    'name' => 'test'
]);

var_dump($app);
//$app->getRequest()->setUri('/index/index/index');
$app->getRequest()->setUri('/index/index/index/aaa/bbb');
$app->run();
