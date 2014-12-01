--TEST--
Check if abstract static classes generates fatal error
--FILE--
<?php

abstract static class A
{
    public static $c = "foo\n";
    public static function display($name)
    {
        echo "Hello $name\n";
    }
}

?>
--EXPECTF--
Fatal error: Cannot use the static modifier on an abstract class in %s on line %d