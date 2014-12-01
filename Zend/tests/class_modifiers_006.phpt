--TEST--
Check if static classes prevents abstract static methods
--FILE--
<?php

final static class A
{
    public static abstract function display($name);
}

?>
--EXPECTF--
Fatal error: Class %s contains abstract method (%s) and therefore cannot be declared 'static' in %s on line %d
