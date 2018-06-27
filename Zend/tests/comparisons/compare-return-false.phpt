--TEST--
__compareTo: Returning false should be converted to integer 0
--FILE--
<?php

class A
{
    public function __compareTo($other)
    {
        return false;
    }
}

var_dump(new A <=> new A);

?>
--EXPECT--
int(0)
