--TEST--
Class constants cannot be inherited from two different interfaces
--FILE--
<?php

interface I1
{
    const C = 1;
}

interface I2
{
    const C = 1;
}

class C implements I1, I2
{
}

?>
--EXPECTF--
Fatal error: Class C inherits both I1::C and I2::C, which is ambiguous in %s on line %d
