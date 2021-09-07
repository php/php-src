--TEST--
Catchable fatal error [2]
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

    set_error_handler('error');

    try {
        blah (new StdClass);
    } catch (Error $ex) {
        echo $ex->getMessage(), "\n";
    }
    echo "ALIVE!\n";
?>
--EXPECTF--
blah(): Argument #1 ($a) must be of type Foo, DynamicObject given, called in %s on line %d
ALIVE!
