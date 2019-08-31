--TEST--
LSP checks are performed against an abstract constructor even if it is not a direct parent
--FILE--
<?php

abstract class A {
    abstract function __construct(X $x);
}
class B extends A {
    function __construct(X $x) {}
}
class C extends B {
    function __construct() {}
}

?>
--EXPECTF--
Fatal error: Declaration of C::__construct() must be compatible with A::__construct(X $x) in %s on line 10
