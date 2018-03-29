--TEST--
Check for linger_framework presence
--SKIPIF--
<?php if (!extension_loaded("linger_framework")) print "skip"; ?>
--FILE--
<?php 
class Boot implements \linger\framework\Bootstrap {
    public function bootstrap(\linger\framework\Application $app) {
    }
}

try {
    $app = new \linger\framework\Application([
        'app_directory' => 'app'
    ]);

    $bootClass = [
        Boot::class
    ];
    $router = $app->getRouter();
    $router->get('/home/@userId:([0-9]+)', "Test", 'test');

    $app->init($bootClass)->run();

} catch (Exception $e) {
    echo "-----------Exception----------\n";
    echo $e->getMessage(),PHP_EOL;
}
?>
--EXPECT--
<h1>404 Not Found</h1>
