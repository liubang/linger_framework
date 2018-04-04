--TEST--
Check for linger_framework presence
--SKIPIF--
<?php if (!extension_loaded("linger_framework")) print "skip"; ?>
--FILE--
<?php 
$app = new linger\framework\Application([
	'app_path' => '/tmp',
]);

$request = new linger\framework\Request();
var_dump($request);
$request->setUri("/home")
->setQuery(['a' => 'test'])
->setParam(['b' => 'test'])
->setPost(['c' => '123']);
var_dump($request->getUri());
var_dump($request->getParam());
var_dump($request->getPost());
var_dump($request->getQuery());
var_dump($request);
var_dump($request->getPost('c', 'default', 'intval'));
var_dump($request->getPost('d', '88888', 'intval'));


?>
--EXPECT--
object(Linger\Framework\Request)#3 (7) {
  ["_method":protected]=>
  NULL
  ["_uri":protected]=>
  NULL
  ["_cookie":protected]=>
  NULL
  ["_query":protected]=>
  NULL
  ["_param":protected]=>
  NULL
  ["_post":protected]=>
  NULL
  ["_files":protected]=>
  NULL
}
string(5) "/home"
array(1) {
  ["b"]=>
  string(4) "test"
}
array(1) {
  ["c"]=>
  string(3) "123"
}
array(1) {
  ["a"]=>
  string(4) "test"
}
object(Linger\Framework\Request)#3 (7) {
  ["_method":protected]=>
  NULL
  ["_uri":protected]=>
  string(5) "/home"
  ["_cookie":protected]=>
  NULL
  ["_query":protected]=>
  array(1) {
    ["a"]=>
    string(4) "test"
  }
  ["_param":protected]=>
  array(1) {
    ["b"]=>
    string(4) "test"
  }
  ["_post":protected]=>
  array(1) {
    ["c"]=>
    string(3) "123"
  }
  ["_files":protected]=>
  NULL
}
int(123)
string(5) "88888"
