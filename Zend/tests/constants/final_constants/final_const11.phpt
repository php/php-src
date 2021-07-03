--TEST--
Class constants cannot be inherited from both a class and an interface
--FILE--
<?php

class C
{
    const C = 1;
}

interface I
{
    const C = 1;
}

class C2 extends C implements I
{
}

?>
--EXPECTF--
Fatal error: Cannot inherit previously-inherited or override constant C2::C from interface I in %s on line %d
