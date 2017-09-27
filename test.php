<?php

$app = new linger\framework\Application(['appname' => 'test']);
var_dump($app);
$config = $app->getConfig();
var_dump($config);
var_dump($app->getDispatcher());
var_dump($config->get('appname'));
$request = $app->getRequest();
var_dump($request);
var_dump($request->getMethod());
echo "COOKIE\n";
var_dump($request->getCookie());
echo "COOKIE\n";

$config = new linger\framework\Config(['bbb' => 'sadfsaf']);
var_dump($config);
var_dump($config->get('bbb'));

$app->getRequest()->setUri('/dsfsa/ttt');
$app->run();
var_dump($app->getDispatcher());
