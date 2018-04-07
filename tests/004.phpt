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
$request->setUri("/home")
->setQuery(['a' => 'test'])
->setParam(['b' => 'test'])
->setPost(['c' => '123']);
var_dump($request->getUri());
var_dump($request->getParam());
var_dump($request->getPost());
var_dump($request->getQuery());
var_dump($request->getPost('c', 'default', 'intval'));
var_dump($request->getPost('d', '88888', 'intval'));


?>
--EXPECT--
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
int(123)
string(5) "88888"
