<?php

/** @generate-function-entries */

final class Closure
{
    private function __construct() {}

    /** @param object|string|null $newScope */
    public static function bind(Closure $closure, ?object $newThis, $newScope = UNKNOWN): ?Closure {}

    /**
     * @param object|string|null $newScope
     * @alias Closure::bind
     */
    public function bindTo(?object $newThis, $newScope = UNKNOWN): ?Closure {}

    public function call(object $newThis, mixed ...$arguments): mixed {}

    /** @param callable $callback callable is not a proper type due to bug #78770. */
    public static function fromCallable($callback): Closure {}
}
