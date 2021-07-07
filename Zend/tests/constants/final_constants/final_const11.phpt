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
Fatal error: Class C2 inherits both C::C and I::C, which is ambiguous in %s on line %d
