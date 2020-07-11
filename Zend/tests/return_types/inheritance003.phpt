--TEST--
Return type mismatch; implements interface
--FILE--
<?php

interface A {
    function foo(): A;
}

class B implements A {
    function foo(): StdClass {}
}
--EXPECTF--
Fatal error: Declaration of B::foo(): StdClass must be compatible with A::foo(): A in %s on line %d
