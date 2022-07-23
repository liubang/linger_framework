<?php

/** @generate-legacy-arginfo */
final class Linger_Framework_RouterRule
{
    private string $_request_method;
    private string $_uri;
    private string $_params_map;
    private string $_class;
    private string $_class_method;

    public function __construct(string $request_method, string $uri, string $class, string $class_method);

    public function getRequestMethod(): string;

    public function getUri(): string;

    public function getClass(): string;

    public function getClassMethod(): string;

    public function dump(): void;
}
