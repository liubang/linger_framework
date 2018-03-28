<?php
namespace linger\framework;

class Application
{
    /**
     * @var Application
     */
	protected static $_app;

    /**
     * @var Config
     */
	protected $_config;

    /**
     * @var Router
     */
	protected $_router;

    /**
     * @var Request
     */
	protected $_request;

    /**
     * @var Dispatcher
     */
	protected $_dispatcher;

    /**
     * Application constructor.
     * @param array $config
     */
	public function __construct(array $config) { }


	public function run() { }

    /**
     * @return $this
     */
	public static function app() { }

    /**
     * @param array $bootarr
     * @return $this
     */
	public function init(array $bootarr) { }

    /**
     * @return Config
     */
	public function getConfig() { }

    /**
     * @return Router
     */
	public function getRouter() { }

    /**
     * @return Config
     */
	public function setConfig() { }

    /**
     * @return Dispatcher
     */
	public function getDispatcher() { }

    /**
     * @return Request
     */
	public function getRequest() { }

	public function __destruct() { }
}
interface Bootstrap
{
    /**
     * @param Application $application
     * @return mixed
     */
	public function bootstrap(Application $application);
}
class Config
{
    /**
     * @var Config
     */
	protected static $_instance;

    /**
     * Config constructor.
     * @param array $config
     */
	public function __construct(array $config) { }

    /**
     * @param null|string $key
     * @return mixed
     */
	public function get($key = null) { }

    /**
     * @param string $key
     * @param mixed $value
     * @return bool
     */
	public function set($key, $value) { }

	public function __destruct() { }
}
abstract class Controller
{
    /**
     * @var Request
     */
	protected $_request;

    /**
     * @var View
     */
	protected $_view;
	private function __construct() { }
	protected function _init() { }

    /**
     * @return Request
     */
	protected function getRequest() { }

    /**
     * @return View
     */
	protected function getView() { }
}
class Dispatcher
{
    /**
     * @var Dispatcher
     */
	protected static $_instance;

    /**
     * @var Router
     */
	private $_router;

    /**
     * @var string
     */
	private $_module;

    /**
     * @var Request
     */
	private $_request;

    /**
     * @var string
     */
	private $_controller;

    /**
     * @var string
     */
	private $_action;
	protected function __construct() { }

    /**
     * @return Request
     */
	public function getRequest() { }

    /**
     * @return RouterRule|false
     */
	public function findRouter() { }
}
class Request
{
    /**
     * @var Request
     */
	protected static $_instance;

    /**
     * @var string
     */
	protected $_method;

    /**
     * @var string
     */
	protected $_uri;

    /**
     * @var array
     */
	protected $_cookie;

    /**
     * @var array
     */
	protected $_query;

    /**
     * @var array
     */
	protected $_param;

    /**
     * @var array
     */
	protected $_post;

    /**
     * @var array
     */
	protected $_files;

    /**
     * Request constructor.
     */
	protected function __construct() { }

    /**
     * @return string
     */
	public function getMethod() { }

    /**
     * @return string
     */
	public function getUri() { }

    /**
     * @param null $key
     * @param null $default
     * @param callable|null $filter
     * @return mixed
     */
	public function getQuery($key = null, $default = null, callable $filter = null) { }

    /**
     * @param null $key
     * @param null $default
     * @param callable|null $filter
     * @return mixed
     */
	public function getParam($key = null, $default = null, callable $filter = null) { }

    /**
     * @param null $key
     * @param null $default
     * @param callable|null $filter
     * @return mixed
     */
	public function getPost($key = null, $default = null, callable $filter = null) { }

    /**
     * @param null $key
     * @return mixed
     */
	public function getCookie($key = null) { }

    /**
     * @return boolean
     */
	public function isGet() { }

    /**
     * @return boolean
     */
	public function isPost() { }

    /**
     * @return boolean
     */
	public function isAjax() { }

    /**
     * @param string $uri
     * @return $this
     */
	public function setUri($uri) { }

    /**
     * @param string $method
     * @return $this
     */
	public function setMethod($method) { }

    /**
     * @param array $query
     * @return $this
     */
	public function setQuery(array $query) { }

    /**
     * @param array $param
     * @return $this
     */
	public function setParam(array $param) { }
}
class Router
{
    /**
     * @var Router
     */
	protected static $_instance;

    /**
     * @var RouterRule
     */
	protected $_rules;
	private function __construct() { }

    /**
     * @param RouterRule $routerRule
     * @return false|$this
     */
	public function add(RouterRule $routerRule) { }

    /**
     * @param string $uri
     * @param string $class
     * @param string $action
     * @return $this
     */
	public function get($uri, $class, $action) { }

    /**
     * @param string $uri
     * @param string $class
     * @param string $action
     * @return $this
     */
	public function post($uri, $class, $action) { }

    /**
     * @param string $uri
     * @param string $class
     * @param string $action
     * @return $this
     */
	public function put($uri, $class, $action) { }

    /**
     * @param string $uri
     * @param string $class
     * @param string $action
     * @return $this
     */
	public function delete($uri, $class, $action) { }
}
class RouterRule
{
    /**
     * @var string
     */
	private $_request_method;

    /**
     * @var string
     */
	private $_uri;

    /**
     * @var string
     */
	private $_class;

    /**
     * @var string
     */
	private $_class_method;

    /**
     * RouterRule constructor.
     */
	public function __construct() { }

    /**
     * @return string
     */
	public function getRequestMethod() { }

	/**
     * @return string
     */
	public function getUri() { }

    /**
     * @return string
     */
	public function getClass() { }

    /**
     * @return string
     */
	public function getClassMethod() { }
}
class View
{
    /**
     * @var array
     */
	protected $_vars;

    /**
     * @var string
     */
	protected $_tpl_dir;

	protected function __construct() { }

    /**
     * @param string $path
     */
	public function setScriptPath($path) { }

    /**
     * @return string
     */
	public function getScriptPath() { }

    /**
     * @param string $key
     * @param mixed $val
     * @return $this
     */
	public function assign($key, $val) { }

    /**
     * @param string $tpl
     * @return boolean
     */
	public function display($tpl) { }

    /**
     * @param string $tpl
     * @return string
     */
	public function render($tpl) { }

    /**
     * @return array
     */
	public function getVars() { }
}

