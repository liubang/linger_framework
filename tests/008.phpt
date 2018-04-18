--TEST--
Check for linger_framework presence
--SKIPIF--
<?php if (!extension_loaded("linger_framework")) print "skip"; ?>
--FILE--
<?php 
$app = new linger\framework\Application([
	'app_path' => '/tmp',
]);

class TestController extends linger\framework\Controller {

	public function _init() {
		echo __METHOD__,PHP_EOL;
	}

	public function index() {
		echo "hello world\n";
	}

	public function testHome() {
		echo "hello world\n";
		var_dump($this->getRequest()->getParam('id', 0, 'intval'));
	}
}

$router = $app->getRouter()
	->get('/home/@id:([0-9]+)', TestController::class, 'testHome');

$req = $app->getDispatcher()
	->getRequest()
	->setParam(['p' => '1234'])
	->setUri('/home/123')
	->setMethod('GET');

$app->run();

?>
--EXPECT--
TestController::_init
hello world
int(123)
