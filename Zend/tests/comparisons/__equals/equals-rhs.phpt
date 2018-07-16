--TEST--
__equals: RHS operator should be considered even if LHS doesn't implement
--FILE--
<?php

class A
{
    // Does not implement __equals 
}

class B
{
    public function __equals($other)
    {
        echo "Comparing!\n";
        return true;
    }
}

var_dump(new A == new B);
var_dump(new B == new A);

?>
--EXPECT--
Comparing!
bool(true)
Comparing!
bool(true)
