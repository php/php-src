--TEST--
Test that a userland class with rigid properties cannot have additional properties written
--FILE--
<?php

class Foo implements RigidProperties {
    public int $x;

    public int $y = 1;
}

$foo = new Foo();
$foo->x = 123; // uninitialized check
$foo->y = 123; // initialized check

try {
    $foo->bar = 123;
}
catch (\Error $e) {
    echo (string)$e . "\n\n";
}

try {
    $foo->bar2 = "abc";
}
catch (\Error $e) {
    echo (string)$e . "\n\n";
}

--EXPECTF--
Error: Cannot add additional property 'bar' to a class with rigid properties in %s:%d
Stack trace:
#0 {main}

Error: Cannot add additional property 'bar2' to a class with rigid properties in %s:%d
Stack trace:
#0 {main}
