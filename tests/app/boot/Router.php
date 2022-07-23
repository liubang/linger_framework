<?php
namespace boot;

use handler\Home;

class Router implements \linger\framework\Bootstrap {
    public function bootstrap(\linger\framework\Application $app): void {
        $app->getRouter()
            ->get('/index.html', Home::class, 'index');
    }
}
