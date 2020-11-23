<?php

/** @generate-function-entries */

final class Closure
{
    private function __construct() {}

    public static function bind(
        Closure $closure,
        ?object $newThis,
        object|string|null $newScope = "static"
    ): ?Closure {}

    public function bindTo(?object $newThis, object|string|null $newScope = "static"): ?Closure {}

    public function call(object $newThis, mixed ...$args): mixed {}

    public static function fromCallable(callable $callback): Closure {}
}
