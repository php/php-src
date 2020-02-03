--TEST--
Testing __callstatic declaration with wrong modifier
--FILE--
<?php

class a
{
    protected static function __callstatic($a, $b)
    {
    }
}

?>
--EXPECTF--
Warning: The magic method __callStatic() must have public visibility and be static in %s on line %d
