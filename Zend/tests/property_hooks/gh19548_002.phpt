--TEST--
GH-19548: Segfault when using inherited properties and opcache (multiple properties)
--FILE--
<?php

interface I1 {
    public mixed $a { get; }
    public mixed $b { get; }
}
class P1 {
    public mixed $a;
    public mixed $b;
}
class C1 extends P1 implements I1 {}

interface I2 {
    public mixed $prop { get; }
}
class P2 {
    public mixed $prop;
}
class Q2 extends P2 {}
class C2 extends Q2 implements I2 {}

echo "Multiple property test passed - no segmentation fault\n";

?>
--EXPECT--
Multiple property test passed - no segmentation fault
