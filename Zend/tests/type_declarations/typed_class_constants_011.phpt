--TEST--
Typed class constants (declaration; nullable)
--FILE--
<?php
class A {
    public const ?int A = null;
}

var_dump(A::A);
?>
--EXPECT--
NULL
