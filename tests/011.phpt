--TEST--
Check for linger_framework presence
--SKIPIF--
<?php if (!extension_loaded("linger_framework")) print "skip"; ?>
--FILE--
<?php 
class Test extends \linger\framework\Controller {
    public function _init() {
        echo "this is init method\n";
    }

    public function test() {
        echo __METHOD__,PHP_EOL;
        $this->getRequest()->setParam([
            'userId' => '111',
            'name' => 'liubang'
        ]); 
        var_dump($this->getRequest()->getMethod());
        var_dump($this->getRequest()->getParam('userId', 0, 'intval'));
        var_dump($this->getRequest()->getQuery('name', 'liubang', 'intval'));
        var_dump($this->getRequest()->getParam('name', 'liubang', 'intval'));
        var_dump($this->getRequest()->getParam());
    }

    public function post() {
        echo __METHOD__,PHP_EOL;
    }
}

try {
    $app = new \linger\framework\Application([
        'app_directory' => 'app'
    ]);

    $app->getRequest()
        ->setMethod('get')
        ->setUri('/home/1234')
        ->setQuery(['name' => 'liubang']);

    $router = $app->getRouter();
    $router->get('/home/@userId:([0-9]+)', "Test", 'test');

    $app->run();

} catch (Exception $e) {
    echo "-----------Exception----------\n";
    echo $e->getMessage(),PHP_EOL;
}
?>
--EXPECT--
this is init method
Test::test
string(3) "get"
int(111)
int(0)
int(0)
array(2) {
  ["userId"]=>
  string(3) "111"
  ["name"]=>
  string(7) "liubang"
}
