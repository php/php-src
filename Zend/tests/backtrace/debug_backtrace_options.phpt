--TEST--
debug_backtrace options
--FILE--
<?php

function backtrace_print($opt = null)
{
    if(is_null($opt)) {
        print_r(debug_backtrace());
    } else {
        print_r(debug_backtrace($opt));
    }
}

function doit($a, $b, $how)
{
    echo "==default\n";
    $how();
    echo "==true\n";
    $how(true);
    echo "==false\n";
    $how(false);
    echo "==DEBUG_BACKTRACE_PROVIDE_OBJECT\n";
    $how(DEBUG_BACKTRACE_PROVIDE_OBJECT);
    echo "==DEBUG_BACKTRACE_IGNORE_ARGS\n";
    $how(DEBUG_BACKTRACE_IGNORE_ARGS);
    echo "==both\n";
    $how(DEBUG_BACKTRACE_PROVIDE_OBJECT|DEBUG_BACKTRACE_IGNORE_ARGS);
}

class foo {
    protected function doCall($dowhat, $how)
    {
       $dowhat('a','b', $how);
    }
    static function statCall($dowhat, $how)
    {
        $obj = new self();
        $obj->doCall($dowhat, $how);
    }
}
foo::statCall("doit", "debug_print_backtrace");
foo::statCall("doit", "backtrace_print");

?>
--EXPECTF--
==default
#0 %sdebug_backtrace_options.php(%d): doit('a', 'b', 'debug_print_bac...')
#1 %sdebug_backtrace_options.php(%d): foo->doCall('doit', 'debug_print_bac...')
#2 %sdebug_backtrace_options.php(%d): foo::statCall('doit', 'debug_print_bac...')
==true
#0 %sdebug_backtrace_options.php(%d): doit('a', 'b', 'debug_print_bac...')
#1 %sdebug_backtrace_options.php(%d): foo->doCall('doit', 'debug_print_bac...')
#2 %sdebug_backtrace_options.php(%d): foo::statCall('doit', 'debug_print_bac...')
==false
#0 %sdebug_backtrace_options.php(%d): doit('a', 'b', 'debug_print_bac...')
#1 %sdebug_backtrace_options.php(%d): foo->doCall('doit', 'debug_print_bac...')
#2 %sdebug_backtrace_options.php(%d): foo::statCall('doit', 'debug_print_bac...')
==DEBUG_BACKTRACE_PROVIDE_OBJECT
#0 %sdebug_backtrace_options.php(%d): doit('a', 'b', 'debug_print_bac...')
#1 %sdebug_backtrace_options.php(%d): foo->doCall('doit', 'debug_print_bac...')
#2 %sdebug_backtrace_options.php(%d): foo::statCall('doit', 'debug_print_bac...')
==DEBUG_BACKTRACE_IGNORE_ARGS
#0 %sdebug_backtrace_options.php(%d): doit()
#1 %sdebug_backtrace_options.php(%d): foo->doCall()
#2 %sdebug_backtrace_options.php(%d): foo::statCall()
==both
#0 %sdebug_backtrace_options.php(%d): doit()
#1 %sdebug_backtrace_options.php(%d): foo->doCall()
#2 %sdebug_backtrace_options.php(%d): foo::statCall()
==default
Array
(
    [0] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => %d
            [function] => backtrace_print
            [args] => Array
                (
                )

        )

    [1] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => %d
            [function] => doit
            [args] => Array
                (
                    [0] => a
                    [1] => b
                    [2] => backtrace_print
                )

        )

    [2] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => %d
            [function] => doCall
            [class] => foo
            [object] => foo Object
                (
                )

            [type] => ->
            [args] => Array
                (
                    [0] => doit
                    [1] => backtrace_print
                )

        )

    [3] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => %d
            [function] => statCall
            [class] => foo
            [type] => ::
            [args] => Array
                (
                    [0] => doit
                    [1] => backtrace_print
                )

        )

)
==true
Array
(
    [0] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => 17
            [function] => backtrace_print
            [args] => Array
                (
                    [0] => 1
                )

        )

    [1] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => %d
            [function] => doit
            [args] => Array
                (
                    [0] => a
                    [1] => b
                    [2] => backtrace_print
                )

        )

    [2] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => %d
            [function] => doCall
            [class] => foo
            [object] => foo Object
                (
                )

            [type] => ->
            [args] => Array
                (
                    [0] => doit
                    [1] => backtrace_print
                )

        )

    [3] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => %d
            [function] => statCall
            [class] => foo
            [type] => ::
            [args] => Array
                (
                    [0] => doit
                    [1] => backtrace_print
                )

        )

)
==false
Array
(
    [0] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => 19
            [function] => backtrace_print
            [args] => Array
                (
                    [0] => 
                )

        )

    [1] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => %d
            [function] => doit
            [args] => Array
                (
                    [0] => a
                    [1] => b
                    [2] => backtrace_print
                )

        )

    [2] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => %d
            [function] => doCall
            [class] => foo
            [type] => ->
            [args] => Array
                (
                    [0] => doit
                    [1] => backtrace_print
                )

        )

    [3] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => %d
            [function] => statCall
            [class] => foo
            [type] => ::
            [args] => Array
                (
                    [0] => doit
                    [1] => backtrace_print
                )

        )

)
==DEBUG_BACKTRACE_PROVIDE_OBJECT
Array
(
    [0] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => 21
            [function] => backtrace_print
            [args] => Array
                (
                    [0] => 1
                )

        )

    [1] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => %d
            [function] => doit
            [args] => Array
                (
                    [0] => a
                    [1] => b
                    [2] => backtrace_print
                )

        )

    [2] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => %d
            [function] => doCall
            [class] => foo
            [object] => foo Object
                (
                )

            [type] => ->
            [args] => Array
                (
                    [0] => doit
                    [1] => backtrace_print
                )

        )

    [3] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => %d
            [function] => statCall
            [class] => foo
            [type] => ::
            [args] => Array
                (
                    [0] => doit
                    [1] => backtrace_print
                )

        )

)
==DEBUG_BACKTRACE_IGNORE_ARGS
Array
(
    [0] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => 23
            [function] => backtrace_print
        )

    [1] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => %d
            [function] => doit
        )

    [2] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => %d
            [function] => doCall
            [class] => foo
            [type] => ->
        )

    [3] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => %d
            [function] => statCall
            [class] => foo
            [type] => ::
        )

)
==both
Array
(
    [0] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => 25
            [function] => backtrace_print
        )

    [1] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => %d
            [function] => doit
        )

    [2] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => %d
            [function] => doCall
            [class] => foo
            [object] => foo Object
                (
                )

            [type] => ->
        )

    [3] => Array
        (
            [file] => %sdebug_backtrace_options.php
            [line] => %d
            [function] => statCall
            [class] => foo
            [type] => ::
        )

)
