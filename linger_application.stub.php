<?php

/** @generate-legacy-arginfo */
final class Linger_Framework_Application
{
    public function __construct(array $config);

    public static function autoload(string $class_name): void;

    public static function app(): object|null;

    public function init(array $classes): object;
    
    public function run(): void;

    public function getConfig(): array;

    public function getRouter(): object;

    public function getDispatcher(): object;
}
