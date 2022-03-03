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
    echo "\n";
    debug_print_backtrace(0, 2);
    echo "\n";
    debug_print_backtrace(0, 0);
    echo "\n";
    debug_print_backtrace(0, 4);
}

a();
?>
--EXPECTF--
#0 %sdebug_print_backtrace_limit.php(7): c()

#0 %sdebug_print_backtrace_limit.php(7): c()
#1 %sdebug_print_backtrace_limit.php(3): b()

#0 %sdebug_print_backtrace_limit.php(7): c()
#1 %sdebug_print_backtrace_limit.php(3): b()
#2 %sdebug_print_backtrace_limit.php(20): a()

#0 %sdebug_print_backtrace_limit.php(7): c()
#1 %sdebug_print_backtrace_limit.php(3): b()
#2 %sdebug_print_backtrace_limit.php(20): a()
