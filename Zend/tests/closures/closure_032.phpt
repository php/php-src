--TEST--
Closure 032: Testing Closure and debug_backtrace
--FILE--
<?php

function test(closure $a) {
        $a(23);
}


$c = function($param) { print_r(debug_backtrace()); debug_print_backtrace(); };

$c(23);
test($c);
?>
--EXPECTF--
Array
(
    [0] => Array
        (
            [file] => %s
            [line] => 10
            [function] => {closure:%s:%d}
            [args] => Array
                (
                    [0] => 23
                )

        )

)
#%d %s(10): {closure:%s:%d}(23)

Fatal error: Uncaught TypeError: test(): Argument #%d ($a) must be of type closure, Closure given, called in %s on line %d and defined in %s:%d
Stack trace:
#%d %s(11): test(Object(Closure))
#%d {main}
  thrown in %s on line %d
