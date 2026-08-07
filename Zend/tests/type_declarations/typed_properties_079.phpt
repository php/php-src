--TEST--
Test static typed properties with references
--FILE--
<?php

class A {
    static iterable $it = [];
    static ?array $a;
}

A::$a = &A::$it;

try {
    A::$it = new ArrayIterator();
} catch (TypeError $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
var_dump(A::$it);

A::$a = &$a;

A::$it = new ArrayIterator();

try {
    $a = 1;
} catch (TypeError $e) { echo $e::class, ': ', $e->getMessage(), "\n"; }
var_dump($a);

?>
--EXPECT--
TypeError: Cannot assign ArrayIterator to reference held by property A::$a of type ?array
array(0) {
}
TypeError: Cannot assign int to reference held by property A::$a of type ?array
NULL
