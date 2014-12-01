--TEST--
Check if final static classes are supported
--FILE--
<?php

final static class A
{
    public static $c = "foo\n";
    public static function display($name)
    {
        echo "Hello $name\n";
    }
}

class B extends A
{
}

?>
--EXPECTF--
Fatal error: Class %s may not inherit from final class (%s) in %s on line %d