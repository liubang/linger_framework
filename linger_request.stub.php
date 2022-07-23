<?php

/** @generate-legacy-arginfo */
class Linger_Framework_Request
{
    protected static object $_instance;
    protected string $_method;
    protected string $_uri;
    protected array $_cookie;
    protected array $_query;
    protected array $_param;
    protected array $_post;
    protected array $_files;

    private function __construct();

    public function getMethod(): string;

    public function getUri(): string;

    public function getQuery(?string $query = "", mixed $default = null,  callable $fileter = null): mixed;

    public function getPost(?string $query = "", mixed $default = null,  callable $fileter = null): mixed;

    public function getParam(?string $query = "", mixed $default = null,  callable $fileter = null): mixed;

    public function getFile(?string $query = "", mixed $default = null,  callable $fileter = null): mixed;

    public function getCookie(?string $query = "", mixed $default = null,  callable $fileter = null): mixed;

    public function isAjax(): bool;

    public function isPost(): bool;

    public function isGet(): bool;

    public function isCli(): bool;

    public function setMethod(string $method): object;

    public function setUri(string $uri): object;

    public function setParam(array $param): object;

    public function setPost(array $post): object;

    public function setQuery(array $query): object;
}
