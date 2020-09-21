<?php

/** @generate-function-entries */

final class Closure
{
    private function __construct() {}

    /** @param object|string|null $newScope */
    public static function bind(Closure $closure, ?object $newThis, $newScope = UNKNOWN): ?Closure {}

    /** @param object|string|null $newScope */
    public function bindTo(?object $newThis, $newScope = UNKNOWN): ?Closure {}

    public function call(object $newThis, mixed ...$arguments): mixed {}

    public static function fromCallable(callable $callback): Closure {}
}
