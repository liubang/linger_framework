<?php
namespace boot;

use handler\Home;

class Router implements \linger\framework\Bootstrap {
    public function bootstrap(\linger\framework\Application $app) {
        $app->getRouter()
            ->get('/index.html', Home::class, 'index');
    }
}
