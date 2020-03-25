<?php

Class Closure
{
    private function __construct() {}

    /** @return ?Closure */
    static function bind(Closure $closure, ?object $newthis, $newscope = UNKNOWN) {}

    /** @return ?Closure */
    function bindTo(?object $newthis, $newscope = UNKNOWN) {}

    function call(object $newthis, ...$parameters) {}

    /**
     * @param callable $callable Not a proper type annotation due to bug #78770
     * @return Closure
     */
    function fromCallable($callable) {}
}
