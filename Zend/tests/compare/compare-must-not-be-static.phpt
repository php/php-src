--TEST--
__compareTo:
--FILE--
<?php

class A
{
    static public function __compareTo()
    {

    }
}

?>
--EXPECTF--
Warning: The magic method __compareTo() must have public visibility and cannot be static in %s on line %d
