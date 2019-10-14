<?php

Class Closure
{
    private function __construct() {}

    /** @return ?Closure */
    static function bind(Closure $closure, ?object $newthis, $newscope = UNKNOWN) {}

    /** @return ?Closure */
    function bindTo(?object $newthis, $newscope = UNKNOWN) {}

    function call(object $newthis, ...$parameters) {}

    /** @return Closure */
    function fromCallable($callable) {}
}
