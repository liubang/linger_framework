--TEST--
Check for linger_framework presence
--SKIPIF--
<?php if (!extension_loaded("linger_framework")) print "skip"; ?>
--FILE--
<?php 

define('APP_PATH', realpath(__DIR__) . '/');

$app = new linger\framework\Application([
	'app_path' => '/tmp',
]);

class TestController extends linger\framework\Controller {

	public function _init() {
		$this->getView()->setScriptPath(APP_PATH . 'html');	
	}

	public function index() {
		$this->getView()->assign('title', 'test')
			->assign('arr', [
					['name' => 'liubang', 'email' => 'it.liubang@gmail.com'],
					['name' => 'liubang', 'email' => 'it.liubang@gmail.com'],
					['name' => 'liubang', 'email' => 'it.liubang@gmail.com'],
			])
			->display('index.phtml');
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
<!doctype html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport"
          content="width=device-width, user-scalable=no, initial-scale=1.0, maximum-scale=1.0, minimum-scale=1.0">
    <meta http-equiv="X-UA-Compatible" content="ie=edge">
    <title>test</title>
</head>
<body>
<header>
this is header
</header>
<header>
this is header
</header>
<h1>hello world</h1>
<ul>
<li>liubang - it.liubang@gmail.com </li>
<li>liubang - it.liubang@gmail.com </li>
<li>liubang - it.liubang@gmail.com </li>
</ul>
</body>
</html>
