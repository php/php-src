--TEST--
Bug #73969: segfault on debug_print_backtrace with require() call
--FILE--
<?php
trait c2
{
    public static function f1()
    {

    }
}

class c1
{
    use c2
    {
        c2::f1 as f2;
    }

    public static function go()
    {
        return require('bug73969.inc');
    }
}

c1::go();
?>
--EXPECTF--
#0 %s(19): require()
#1 %s(23): c1::go()
