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
Fatal error: Cannot inherit previously-inherited or override constant I3::C from interface I2 in %s on line %d
