--TEST--
Check for linger_framework presence
--SKIPIF--
<?php if (!extension_loaded("linger_framework")) print "skip"; ?>
--FILE--
<?php 
$config = new \linger\framework\Config([
    'name' => 'liubang',
    'age'  => 24
]);

print_r($config);

var_dump($config->get());
var_dump($config->get('name'));
var_dump($config->get('age'));
?>
--EXPECT--
Linger\Framework\Config Object
(
    [_config] => Array
        (
            [name] => liubang
            [age] => 24
        )

)
array(2) {
  ["name"]=>
  string(7) "liubang"
  ["age"]=>
  int(24)
}
string(7) "liubang"
int(24)
