--TEST--
debug_print_backtrace limit
--FILE--
<?php
function a() {
    b();
}

function b() {
    c();
}

function c() {
    debug_print_backtrace(0, 1);
    debug_print_backtrace(0, 2);
    debug_print_backtrace(0, 0);
    debug_print_backtrace(0, 4);
}

a();
?>
--EXPECTF--
#0  c() called at [%sdebug_print_backtrace_limit.php:7]
#0  c() called at [%sdebug_print_backtrace_limit.php:7]
#1  b() called at [%sdebug_print_backtrace_limit.php:3]
#0  c() called at [%sdebug_print_backtrace_limit.php:7]
#1  b() called at [%sdebug_print_backtrace_limit.php:3]
#2  a() called at [%sdebug_print_backtrace_limit.php:17]
#0  c() called at [%sdebug_print_backtrace_limit.php:7]
#1  b() called at [%sdebug_print_backtrace_limit.php:3]
#2  a() called at [%sdebug_print_backtrace_limit.php:17]
