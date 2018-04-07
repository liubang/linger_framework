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
?>
--EXPECT--

Fatal error: Uncaught Error: Call to private Linger\framework\Controller::__construct() from invalid context in /home/liubang/workspace/c/linger_framework/tests/005.php:12
Stack trace:
#0 {main}
  thrown in /home/liubang/workspace/c/linger_framework/tests/005.php on line 12
