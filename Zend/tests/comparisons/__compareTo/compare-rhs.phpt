--TEST--
__compareTo: RHS operator should be considered even if LHS doesn't implement
--FILE--
<?php

class A
{
    // Does not implement __compareTo
}

class B
{
    public function __compareTo($other)
    {
        echo "Comparing!\n";
        return 1;
    }
}

var_dump(new A <=> new B);
var_dump(new B <=> new A);

?>
--EXPECT--
Comparing!
int(-1)
Comparing!
int(1)
