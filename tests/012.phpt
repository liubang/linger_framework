--TEST--
Check for linger_framework presence
--SKIPIF--
<?php if (!extension_loaded("linger_framework")) print "skip"; ?>
--FILE--
<?php 

define('APP_PATH', realpath(__DIR__) . '/');

$app = new linger\framework\Application([
]);

class TestController extends linger\framework\Controller {

	public function index() {
		print_r($this->getRequest()->getParam());
	}

}

$router = $app->getRouter()
	->get('/home/@id:([0-9]+)', TestController::class, 'index');

for ($i = 0; $i < 100; ++$i) {
    $router->get('/home/@name:([a-z]+)/@id:(\d+)', TestController::class, 'index')
        ->put('/home/@name:([a-z]+)/@id:(\d+)', TestController::class, 'index')
        ->post('/home/@name:([a-z]+)/@id:(\d+)', TestController::class, 'index')
        ->delete('/home/@name:([a-z]+)/@id:(\d+)', TestController::class, 'index');
}

$router->get('/home/@name:([a-z]+)/@age:(\d+)/@id:(\d+)', TestController::class, 'index');

$req = $app->getDispatcher()
	->getRequest()
	->setUri('/home/liubang/25/88888')
	->setMethod('GET');

$app->run();

?>
--EXPECT--
Array
(
    [name] => liubang
    [age] => 25
    [id] => 88888
)
