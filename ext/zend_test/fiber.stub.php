<?php

/**
 * @generate-class-entries static
 * @undocumentable
 */

final class _ZendTestFiber
{
    public function __construct(callable $callback) {}

    public function start(mixed ...$args): mixed {}

    public function resume(mixed $value = null): mixed {}

    public function pipeTo(callable $callback): _ZendTestFiber {}

    public static function suspend(mixed $value = null): mixed {}
}
