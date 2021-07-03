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
Fatal error: Cannot inherit previously-inherited or override constant C::C from interface I2 in %s on line %d
