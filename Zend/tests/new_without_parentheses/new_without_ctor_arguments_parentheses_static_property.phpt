--TEST--
Immediate static property access on new object without constructor parentheses
--FILE--
<?php

class B
{
}

class A
{
    public static $prop = B::class;
}

var_dump(new A::$prop);

?>
--EXPECT--
object(B)#1 (0) {
}
