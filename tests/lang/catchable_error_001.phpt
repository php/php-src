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
Fatal error: Uncaught TypeError: blah(): Argument #1 ($a) must be of type Foo, stdClass given, called in %scatchable_error_001.php on line 15 and defined in %scatchable_error_001.php:5
Stack trace:
#0 %s(%d): blah(Object(stdClass))
#1 {main}
  thrown in %scatchable_error_001.php on line 5
