<?php

/** @generate-legacy-arginfo */
class Linger_Framework_Router
{
    private static object $_instance;
    private array $_rules;
    private int $_chunk_size;

    private function __construct();

    public function add(object $rule): object;

    public function get(string $uri, string $clazz, string $method): object;

    public function put(string $uri, string $clazz, string $method): object;

    public function post(string $uri, string $clazz, string $method): object;

    public function delete(string $uri, string $clazz, string $method): object;

    public function setChunkSize(int $size): object;

    public function getRules(): array;

    public function dump(): void;
}
