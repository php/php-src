--TEST--
Typed class constants (declaration; simple)
--FILE--
<?php
class A {
    public const int A = 1;
}

echo A::A;
?>
--EXPECT--
1
