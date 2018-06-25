--TEST--
__compareTo: Returning an object should be converted to int
--FILE--
<?php

class A
{
    public function __compareTo($other)
    {
        return new A();
    }
}

var_dump(new A <=> new A);

?>
--EXPECTF--
Notice: Object of class A could not be converted to int in %s on line %d
int(1)
