--TEST--
Bug #73987 (Method compatibility check looks to original definition and not parent - nullabilty abstract)
--FILE--
<?php

abstract class A {
    abstract function example($a, $b, $c);
}

class B extends A {
    function example($a, $b = null, $c = null) { }
}

class C extends B {
    function example($a, $b, $c = null) { }
}

?>
--EXPECTF--
Fatal error: Declaration of C::example($a, $b, $c = NULL) must be compatible with B::example($a, $b = NULL, $c = NULL) in %s
