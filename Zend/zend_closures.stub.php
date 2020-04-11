<?php

Class Closure
{
    private function __construct() {}

    /** @return ?Closure */
    public static function bind(Closure $closure, ?object $newthis, $newscope = UNKNOWN) {}

    /** @return ?Closure */
    public function bindTo(?object $newthis, $newscope = UNKNOWN) {}

    /** @return mixed */
    public function call(object $newthis, ...$parameters) {}

    /**
     * @param callable $callable Not a proper type annotation due to bug #78770
     * @return Closure
     */
    public function fromCallable($callable) {}
}
