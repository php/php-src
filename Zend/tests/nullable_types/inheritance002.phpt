--TEST--
Nullable parameter type inheritance rules (nullable and not-nullable)
--FILE--
<?php
class A {
    function foo(?array $a) {}
}

class B extends A {
    function foo(array $a) {}
}
?>
DONE
--EXPECTF--
Strict Standards: Declaration of B::foo() should be compatible with A::foo(?array $a) in %sinheritance002.php on line 8
DONE
