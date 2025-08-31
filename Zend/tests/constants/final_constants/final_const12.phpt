--TEST--
Interface constants cannot be inherited from other interfaces
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
}

?>
--EXPECTF--
Fatal error: Interface I3 inherits both I1::C and I2::C, which is ambiguous in %s on line %d
