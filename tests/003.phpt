--TEST--
Check for linger_framework presence
--SKIPIF--
<?php if (!extension_loaded("linger_framework")) print "skip"; ?>
--FILE--
<?php 
try {
    $app = new \linger\framework\Application([

    ]);
} catch (Exception $e) {
    echo "-----------Exception----------\n";
    echo $e->getMessage(),PHP_EOL;
}
 
?>
--EXPECT--
-----------Exception----------
must set app_directory in config.
