--TEST--
Check for linger_framework presence
--SKIPIF--
<?php if (!extension_loaded("linger_framework")) print "skip"; ?>
--FILE--
<?php 
try {
    $app = new \linger\framework\Application([
        'app_directory' => 'app'
    ]);

    $request = $app->getRequest();
    print_r($request);

    $request->setMethod('get');
    var_dump($request->getMethod());
    var_dump($request->isGet());
    var_dump($request->isPost());
    var_dump($request->isAjax());

    $request->setUri('/home');
    var_dump($request->getUri());

} catch (Exception $e) {
    echo "-----------Exception----------\n";
    echo $e->getMessage(),PHP_EOL;
} 
?>
--EXPECT--
Linger\Framework\Request Object
(
    [_method:protected] => CLI
    [_uri:protected] => 
    [_cookie:protected] => Array
        (
        )

    [_query:protected] => Array
        (
        )

    [_param:protected] => Array
        (
        )

    [_post:protected] => Array
        (
        )

    [_files:protected] => Array
        (
        )

)
string(3) "get"
bool(true)
bool(false)
bool(false)
string(5) "/home"
