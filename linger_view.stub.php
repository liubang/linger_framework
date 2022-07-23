<?php

/** @generate-legacy-arginfo */
class Linger_Framework_View
{
    private array $_vars;
    private string $_tpl_dir;

    public function __construct();

    public function setScriptPath(string $path): object;

    public function getScriptPath(): string;

    public function display(string $tpl): void;

    public function render(string $tpl): string;

    public function getVars(string $name): mixed;

    public function assign(string $name, mixed $val): object;
}
