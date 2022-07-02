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
#0 %sdebug-backtrace.php(9): inner_function()
#1 [internal function]: {closure}()
#2 %sdebug-backtrace.php(12): Fiber->start()
