--TEST--
Nullable return type inheritance rules (nullable and not-nullable)
--FILE--
<?php
class A {
    function foo() : ?array { return []; }
}

class B extends A {
    function foo() : array { return []; }
}
?>
DONE
--EXPECT--
DONE
