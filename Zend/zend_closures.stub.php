<?php

/** @generate-function-entries */

final class Closure
{
    private function __construct() {}

    public static function bind(Closure $closure, ?object $newthis, $newscope = UNKNOWN): ?Closure {}

    /** @alias Closure::bind */
    public function bindTo(?object $newthis, $newscope = UNKNOWN): ?Closure {}

    public function call(object $newthis, mixed ...$parameters): mixed {}

    /** @param callable $callable Not a proper type annotation due to bug #78770 */
    public static function fromCallable($callable): Closure {}
}
