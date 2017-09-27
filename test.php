<?php

$app = new linger\framework\Application(['appname' => 'test']);
var_dump($app);
$config = $app->getConfig();
var_dump($config);
var_dump($app->getDispatcher());
var_dump($config->get('appname'));
var_dump($app->getRequest());


$config = new linger\framework\Config(['bbb' => 'sadfsaf']);
var_dump($config);
var_dump($config->get('bbb'));
