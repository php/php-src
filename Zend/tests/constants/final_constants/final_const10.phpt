--TEST--
Interface constants inherited from other interfaces can be redeclared
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

interface I3 extends I1, I2
{
    const C = 3;
}

?>
--EXPECT--
