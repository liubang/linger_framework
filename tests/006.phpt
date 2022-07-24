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
		echo __METHOD__;
	}
}

$req = $app->getDispatcher()->getRequest()->setUri('/home')
	->setMethod('GET')
	->setParam(['p' => '1234'])
	->setQuery(['q' => 'test'])
	->setPost(['p' => 'test']);

$app->run();

?>
--EXPECT--
