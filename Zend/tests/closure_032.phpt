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
            [line] => %d
            [function] => {closure}
            [args] => Array
                (
                    [0] => 23
                )

        )

)
#0  {closure}(23) called at [%s:%d]
Array
(
    [0] => Array
        (
            [file] => %s
            [line] => %d
            [function] => {closure}
            [args] => Array
                (
                    [0] => 23
                )

        )

    [1] => Array
        (
            [file] => %s
            [line] => %d
            [function] => test
            [args] => Array
                (
                    [0] => Closure Object
                        (
                            [parameter] => Array
                                (
                                    [$param] => <required>
                                )

                        )

                )

        )

)
#0  {closure}(23) called at [%s:%d]
#1  test(Closure Object ()) called at [%s:%d]

