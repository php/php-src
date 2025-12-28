--TEST--
Bug #73987 (Method compatibility check looks to original definition and not parent - return types abstract)
--FILE--
<?php

abstract class A {
    abstract function example();
}

class B extends A {
    function example(): int  { }
}

class C extends B {
    function example(): string { }
}

?>
--EXPECTF--
Fatal error: Declaration of C::example(): string must be compatible with B::example(): int in %s
