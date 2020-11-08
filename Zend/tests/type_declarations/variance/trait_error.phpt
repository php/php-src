--TEST--
Trait delayed variance check fails
--FILE--
<?php

// Taken from bug #78989.

class X {
    function method($a): A {}
}
trait T {
    function method($r): B {}
}
class U extends X {
    use T;
}

?>
--EXPECTF--
Fatal error: Could not check compatibility between T::method($r): B and X::method($a): A, because class B is not available in %s on line %d
