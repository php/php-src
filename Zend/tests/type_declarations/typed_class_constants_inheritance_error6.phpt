--TEST--
Typed class constants (type widening iface class const)
--FILE--
<?php

interface A {
    public const string CONST1 = 'A';
}

class B implements A {
    public const CONST1 = 'B';
}

?>
--EXPECTF--
Fatal error: Type of B::CONST1 must be compatible with A::CONST1 of type string in %s on line %d
