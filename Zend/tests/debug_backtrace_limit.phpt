--TEST--
debug_backtrace limit
--FILE--
<?php
function a() {
    b();
}

function b() {
    c();
}

function c() {
    print_r(debug_backtrace(0, 1));
    print_r(debug_backtrace(0, 2));
    print_r(debug_backtrace(0, 0));
    print_r(debug_backtrace(0, 4));
}

a();
?>
--EXPECTF--
Array
(
    [0] => Array
        (
            [file] => %sdebug_backtrace_limit.php
            [line] => 7
            [function] => c
            [args] => Array
                (
                )

        )

)
Array
(
    [0] => Array
        (
            [file] => %sdebug_backtrace_limit.php
            [line] => 7
            [function] => c
            [args] => Array
                (
                )

        )

    [1] => Array
        (
            [file] => %sdebug_backtrace_limit.php
            [line] => 3
            [function] => b
            [args] => Array
                (
                )

        )

)
Array
(
    [0] => Array
        (
            [file] => %sdebug_backtrace_limit.php
            [line] => 7
            [function] => c
            [args] => Array
                (
                )

        )

    [1] => Array
        (
            [file] => %sdebug_backtrace_limit.php
            [line] => 3
            [function] => b
            [args] => Array
                (
                )

        )

    [2] => Array
        (
            [file] => %sdebug_backtrace_limit.php
            [line] => 17
            [function] => a
            [args] => Array
                (
                )

        )

)
Array
(
    [0] => Array
        (
            [file] => %sdebug_backtrace_limit.php
            [line] => 7
            [function] => c
            [args] => Array
                (
                )

        )

    [1] => Array
        (
            [file] => %sdebug_backtrace_limit.php
            [line] => 3
            [function] => b
            [args] => Array
                (
                )

        )

    [2] => Array
        (
            [file] => %sdebug_backtrace_limit.php
            [line] => 17
            [function] => a
            [args] => Array
                (
                )

        )

)
