<?php

$app = new linger\framework\Application(['appname' => 'test']);
var_dump($app);
var_dump($app->getConfig());
var_dump($app->getDispatcher());
