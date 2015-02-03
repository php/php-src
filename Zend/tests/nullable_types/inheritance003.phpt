--TEST--
Nullable parameter type inheritance rules (not-nullable and nullable)
--FILE--
<?php
class A {
    function foo(array $a) {}
}

class B extends A {
    function foo(?array $a) {}
}
?>
DONE
--EXPECT--
DONE
