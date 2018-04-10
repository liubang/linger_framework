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
		$this->getResponse()
			->status(200)
			->json([
					'name' => 'liubang',
					'age'  => '25'
			])->send();
	}

}

$router = $app->getRouter()
	->get('/home/@id:([0-9]+)', TestController::class, 'index');

$req = $app->getDispatcher()
	->getRequest()
	->setUri('/home/123')
	->setMethod('GET');

$app->run();

?>
--EXPECT--
{"name":"liubang","age":"25"}
