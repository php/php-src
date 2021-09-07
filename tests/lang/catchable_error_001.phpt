--TEST--
Catchable fatal error [1]
--FILE--
<?php
    class Foo {
    }

    function blah (Foo $a)
    {
    }

    function error()
    {
        $a = func_get_args();
        var_dump($a);
    }

    blah (new StdClass);
    echo "ALIVE!\n";
?>
--EXPECTF--
Fatal error: Uncaught TypeError: blah(): Argument #1 ($a) must be of type Foo, DynamicObject given, called in %s:%d
Stack trace:
#0 %s(%d): blah(Object(DynamicObject))
#1 {main}
  thrown in %scatchable_error_001.php on line 5
