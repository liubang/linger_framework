--TEST--
Check for linger_framework presence
--SKIPIF--
<?php if (!extension_loaded("linger_framework")) print "skip"; ?>
--FILE--
<?php 

define('APP_PATH', realpath(__DIR__) . '/');

$app = new linger\framework\Application([
		'app_directory' => '/tmp'
]);

class TestController extends linger\framework\Controller {

	public function index() {
		$app = linger\framework\Application::app();
		$routerRule = $app->getDispatcher()->findRouter();
		var_dump($routerRule);
	}

}

$router = $app->getRouter()
	->get('/home/@id:([0-9]+)', TestController::class, 'index');

$req = $app->getDispatcher()
	->getRequest()
	->setUri('/home/123')
	->setMethod('GET');

$app->run();

?>
--EXPECT--
object(Linger\Framework\RouterRule)#6 (5) {
  ["_request_method":"Linger\Framework\RouterRule":private]=>
  string(3) "get"
  ["_uri":"Linger\Framework\RouterRule":private]=>
  string(18) "/home/@id:([0-9]+)"
  ["_params_map":"Linger\Framework\RouterRule":private]=>
  array(1) {
    [0]=>
    string(2) "id"
  }
  ["_class":"Linger\Framework\RouterRule":private]=>
  string(14) "TestController"
  ["_class_method":"Linger\Framework\RouterRule":private]=>
  string(5) "index"
}
