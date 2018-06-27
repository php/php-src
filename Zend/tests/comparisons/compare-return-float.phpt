--TEST--
__compareTo: Returning a float should not be truncated to integer
--FILE--
<?php

class A
{
    public function __compareTo($other)
    {
        return 0.5;
    }
}

class B
{
    public function __compareTo($other)
    {
        return -0.5;
    }
}

class C
{
    public function __compareTo($other)
    {
        return 0.0;
    }
}

var_dump(new A <=> 1);
var_dump(new B <=> 2);
var_dump(new C <=> 3);

?>
--EXPECT--
int(1)
int(-1)
int(0)
