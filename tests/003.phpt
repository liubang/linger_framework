--TEST--
Check for linger_framework presence
--SKIPIF--
<?php if (!extension_loaded("linger_framework")) print "skip"; ?>
--FILE--
<?php 
$app = new linger\framework\Application([
	'app_path' => '/tmp',
]);

class TestBootstrapA implements linger\framework\Bootstrap {
	public function bootstrap(linger\framework\Application $app) {
		echo __METHOD__, PHP_EOL;	
	}
}

class TestBootstrapB implements linger\framework\Bootstrap {
	public function bootstrap(linger\framework\Application $app) {
		echo __METHOD__, PHP_EOL;	
	}
}

$boots = [
	TestBootstrapA::class,
	TestBootstrapB::class,
];
$app->init($boots);
?>
--EXPECT--
TestBootstrapA::bootstrap
TestBootstrapB::bootstrap
