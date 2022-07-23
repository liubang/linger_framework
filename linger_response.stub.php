<?php

/** @generate-legacy-arginfo */
class Linger_Framework_Response
{
    protected static object $_instance;
    protected array $_header;
    protected int $_status;
    protected string $_body;

    private function __construct();

    public function status(int $status): object;

    public function header(string $key, string $val): object;

    public function body(string $body): object;

    public function send(): void;

    public function json(mixed $value): void;

    public function redirect(string $url): void;
}
