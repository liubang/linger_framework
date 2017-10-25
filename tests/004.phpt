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

    print_r($app);
} catch (Exception $e) {
    echo "-----------Exception----------\n";
    echo $e->getMessage(),PHP_EOL;
}

 
?>
--EXPECT--
Linger\Framework\Application Object
(
    [_config:protected] => Linger\Framework\Config Object
        (
            [_config] => Array
                (
                    [app_directory] => app
                )

        )

    [_router:protected] => Linger\Framework\Router Object
        (
            [_rules:protected] => Array
                (
                )

        )

    [_request:protected] => Linger\Framework\Request Object
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

    [_dispatcher:protected] => Linger\Framework\Dispatcher Object
        (
            [_router:Linger\Framework\Dispatcher:private] => Linger\Framework\Router Object
                (
                    [_rules:protected] => Array
                        (
                        )

                )

            [_module:Linger\Framework\Dispatcher:private] => 
            [_request:Linger\Framework\Dispatcher:private] => Linger\Framework\Request Object
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

            [_controller:Linger\Framework\Dispatcher:private] => 
            [_action:Linger\Framework\Dispatcher:private] => 
        )

)
