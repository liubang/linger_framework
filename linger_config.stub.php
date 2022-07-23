<?php

/** @generate-legacy-arginfo */
final class Linger_Framework_Config implements \Iterator, \Traversable, \ArrayAccess, \Countable
{
    public function __construct(array $config);

    public function get(?string $name = null): mixed;

    public function set(string $name, mixed $val): object;

    public function has(string $name): bool;

    public function del(string $name): object|false;

    public function clear(): void;


    public function count(): int;

    public function rewind(): void;

    public function current(): mixed;

    public function next(): void;

    public function valid(): bool;

    public function key(): int|string|null|bool;

    public function offsetGet(mixed $name): mixed;

    public function offsetSet(mixed $name, mixed $value): void;

    public function offsetUnSet(mixed $name): void;

    public function offsetExists(mixed $name): bool;

    public function __isset(string $name): bool;

    public function __get(string $name): mixed;

    public function __set(string $name, mixed $value): void;

    public function __unset(string $name): void;
}
