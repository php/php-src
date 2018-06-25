--TEST--
__compareTo: Returning true should be converted to integer 1
--FILE--
<?php

class A
{
    public function __compareTo($other)
    {
        return true;
    }
}

var_dump(new A <=> new A);

?>
--EXPECTF--
int(1)
