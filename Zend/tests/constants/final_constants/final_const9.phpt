--TEST--
Class constants inherited from interfaces can be redeclared
--FILE--
<?php

interface I1
{
    const C = 1;
}

interface I2
{
    const C = 2;
}

class C implements I1, I2
{
    const C = 3;
}

?>
--EXPECT--
