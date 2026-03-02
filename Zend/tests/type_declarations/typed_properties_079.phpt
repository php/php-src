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
} catch (TypeError $e) { var_dump($e->getMessage()); }
var_dump(A::$it);

A::$a = &$a;

A::$it = new ArrayIterator();

try {
    $a = 1;
} catch (TypeError $e) { var_dump($e->getMessage()); }
var_dump($a);

?>
--EXPECT--
string(78) "Cannot assign ArrayIterator to reference held by property A::$a of type ?array"
array(0) {
}
string(68) "Cannot assign int to reference held by property A::$a of type ?array"
NULL
