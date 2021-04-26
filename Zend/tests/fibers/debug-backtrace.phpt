--TEST--
Print backtrace in fiber
--FILE--
<?php

function inner_function(): void
{
    debug_print_backtrace();
}

$fiber = new Fiber(function (): void {
    inner_function();
});

$fiber->start();

?>
--EXPECTF--
#0  inner_function() called at [%sdebug-backtrace.php:9]
#1  {closure}()
#2  Fiber->start() called at [%sdebug-backtrace.php:12]
