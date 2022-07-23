--TEST--
Check for linger_framework presence
--SKIPIF--
<?php if (!extension_loaded("linger_framework")) print "skip"; ?>
--FILE--
<?php 
$app = new linger\framework\Application([
	'app_directory' => '/tmp',
]);

class TestController extends linger\framework\Controller {

	public function _init(): void {
		echo __METHOD__,PHP_EOL;
	}

	public function index() {
		echo "hello world\n";
	}
}

$router = $app->getRouter()
	->add(new Linger\framework\RouterRule('get', '/home', TestController::class, 'index'));

$req = $app->getDispatcher()->getRequest()->setUri('/home')
	->setMethod('GET')
	->setParam(['p' => '1234'])
	->setQuery(['q' => 'test'])
	->setPost(['p' => 'test']);

$app->run();

?>
--EXPECT--
TestController::_init
hello world
