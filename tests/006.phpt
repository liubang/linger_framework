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

    $router = $app->getRouter();
    print_r($router);

    $router->get('/home', "Test", 'test')
        ->post('/home', 'Test', 'post')
        ->put('/home', "Test", 'put')
        ->delete('/home', 'Test', 'delete');

    print_r($router);

    echo "-------------add router rule--------------\n";

    $routerRule = new \linger\framework\RouterRule('get', '/test/routerRule', 'Test', 'test');
    $router->add($routerRule);
    print_r($router);

} catch (Exception $e) {
    echo "-----------Exception----------\n";
    echo $e->getMessage(),PHP_EOL;
}
?>
--EXPECT--
Linger\Framework\Router Object
(
    [_rules:protected] => Array
        (
        )

)
Linger\Framework\Router Object
(
    [_rules:protected] => Array
        (
            [0] => Linger\Framework\RouterRule Object
                (
                    [_request_method:Linger\Framework\RouterRule:private] => get
                    [_uri:Linger\Framework\RouterRule:private] => /home/
                    [_class:Linger\Framework\RouterRule:private] => Test
                    [_class_method:Linger\Framework\RouterRule:private] => test
                )

            [1] => Linger\Framework\RouterRule Object
                (
                    [_request_method:Linger\Framework\RouterRule:private] => post
                    [_uri:Linger\Framework\RouterRule:private] => /home/
                    [_class:Linger\Framework\RouterRule:private] => Test
                    [_class_method:Linger\Framework\RouterRule:private] => post
                )

            [2] => Linger\Framework\RouterRule Object
                (
                    [_request_method:Linger\Framework\RouterRule:private] => put
                    [_uri:Linger\Framework\RouterRule:private] => /home/
                    [_class:Linger\Framework\RouterRule:private] => Test
                    [_class_method:Linger\Framework\RouterRule:private] => put
                )

            [3] => Linger\Framework\RouterRule Object
                (
                    [_request_method:Linger\Framework\RouterRule:private] => delete
                    [_uri:Linger\Framework\RouterRule:private] => /home/
                    [_class:Linger\Framework\RouterRule:private] => Test
                    [_class_method:Linger\Framework\RouterRule:private] => delete
                )

        )

)
-------------add router rule--------------
Linger\Framework\Router Object
(
    [_rules:protected] => Array
        (
            [0] => Linger\Framework\RouterRule Object
                (
                    [_request_method:Linger\Framework\RouterRule:private] => get
                    [_uri:Linger\Framework\RouterRule:private] => /home/
                    [_class:Linger\Framework\RouterRule:private] => Test
                    [_class_method:Linger\Framework\RouterRule:private] => test
                )

            [1] => Linger\Framework\RouterRule Object
                (
                    [_request_method:Linger\Framework\RouterRule:private] => post
                    [_uri:Linger\Framework\RouterRule:private] => /home/
                    [_class:Linger\Framework\RouterRule:private] => Test
                    [_class_method:Linger\Framework\RouterRule:private] => post
                )

            [2] => Linger\Framework\RouterRule Object
                (
                    [_request_method:Linger\Framework\RouterRule:private] => put
                    [_uri:Linger\Framework\RouterRule:private] => /home/
                    [_class:Linger\Framework\RouterRule:private] => Test
                    [_class_method:Linger\Framework\RouterRule:private] => put
                )

            [3] => Linger\Framework\RouterRule Object
                (
                    [_request_method:Linger\Framework\RouterRule:private] => delete
                    [_uri:Linger\Framework\RouterRule:private] => /home/
                    [_class:Linger\Framework\RouterRule:private] => Test
                    [_class_method:Linger\Framework\RouterRule:private] => delete
                )

            [4] => Linger\Framework\RouterRule Object
                (
                    [_request_method:Linger\Framework\RouterRule:private] => get
                    [_uri:Linger\Framework\RouterRule:private] => /test/routerRule/
                    [_class:Linger\Framework\RouterRule:private] => Test
                    [_class_method:Linger\Framework\RouterRule:private] => test
                )

        )

)
