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
		echo __METHOD__;
	}
}

$c = new TestController();
var_dump($c);


?>
--EXPECT--
