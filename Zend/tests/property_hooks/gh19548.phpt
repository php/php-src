--TEST--
GH-19548: Segfault when using inherited properties and opcache
--FILE--
<?php

interface I {
    public mixed $i { get; }
}
class P {
    public mixed $i;
}

class C extends P implements I {}

echo "Test passed - no segmentation fault\n";

?>
--EXPECT--
Test passed - no segmentation fault
