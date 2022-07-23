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
var_dump($req);

var_dump($req->getParam('p', 0, 'intval'));
var_dump($req->getParam('a', 0, 'intval'));


?>
--EXPECT--
object(Linger\Framework\Request)#5 (7) {
  ["_method":protected]=>
  string(3) "get"
  ["_uri":protected]=>
  string(5) "/home"
  ["_cookie":protected]=>
  array(0) {
  }
  ["_query":protected]=>
  array(1) {
    ["q"]=>
    string(4) "test"
  }
  ["_param":protected]=>
  array(1) {
    ["p"]=>
    string(4) "1234"
  }
  ["_post":protected]=>
  array(1) {
    ["p"]=>
    string(4) "test"
  }
  ["_files":protected]=>
  array(0) {
  }
}
int(1234)
int(0)
