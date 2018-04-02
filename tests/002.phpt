--TEST--
Check for linger_framework presence
--SKIPIF--
<?php if (!extension_loaded("linger_framework")) print "skip"; ?>
--FILE--
<?php 
$app = new \linger\framework\Application([
	'app_path' => '/tmp',
	'db' => [
		'host' => 'localhost',
		'name' => 'test',
		'pass' => '123456'
	]
]);
echo "-----object------\n";
var_dump($app->getConfig());
$config = $app->getConfig();
echo "-----array-------\n";
var_dump($config->get());
echo "-----foreach------\n";
foreach ($config as $key => $val) {
	echo $key, PHP_EOL;
	var_dump($val);
}

var_dump($config['app_path']);
?>
--EXPECT--
-----object------
object(Linger\Framework\Config)#2 (1) {
  ["_config":protected]=>
  array(2) {
    ["app_path"]=>
    string(4) "/tmp"
    ["db"]=>
    array(3) {
      ["host"]=>
      string(9) "localhost"
      ["name"]=>
      string(4) "test"
      ["pass"]=>
      string(6) "123456"
    }
  }
}
-----array-------
array(2) {
  ["app_path"]=>
  string(4) "/tmp"
  ["db"]=>
  array(3) {
    ["host"]=>
    string(9) "localhost"
    ["name"]=>
    string(4) "test"
    ["pass"]=>
    string(6) "123456"
  }
}
-----foreach------
app_path
string(4) "/tmp"
db
array(3) {
  ["host"]=>
  string(9) "localhost"
  ["name"]=>
  string(4) "test"
  ["pass"]=>
  string(6) "123456"
}
string(4) "/tmp"
