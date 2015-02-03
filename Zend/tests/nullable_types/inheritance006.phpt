--TEST--
Nullable return type inheritance rules (not-nullable and nullable)
--FILE--
<?php
class A {
    function foo() : array { return []; }
}

class B extends A {
    function foo() : ?array { return []; }
}
?>
DONE
--EXPECTF--
Fatal error: Declaration of B::foo() must be compatible with A::foo(): array in %sinheritance006.php on line 8
