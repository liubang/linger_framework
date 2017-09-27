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


$config = new linger\framework\Config(['bbb' => 'sadfsaf']);
var_dump($config);
var_dump($config->get('bbb'));
