--TEST--
Check for linger_framework presence
--SKIPIF--
<?php if (!extension_loaded("linger_framework")) print "skip"; ?>
--FILE--
<?php 
$app = new linger\framework\Application([
	'app_directory' => '/tmp',
]);

class TestBootstrapA implements linger\framework\Bootstrap {
	public function bootstrap(linger\framework\Application $app): void {
		echo __METHOD__, PHP_EOL;	
	}
}

class TestBootstrapB implements linger\framework\Bootstrap {
	public function bootstrap(linger\framework\Application $app): void {
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
