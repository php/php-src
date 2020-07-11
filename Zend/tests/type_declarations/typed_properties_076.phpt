--TEST--
Computation of intersection types for typed reference to typed property assignments
--FILE--
<?php

class A {}
class B extends A {}

class Test {
    public int $int;
    public float $float;
    public ?int $nint;
    public ?string $nstring;
    public array $array;
    public object $object;
    public iterable $iterable;
    public Iterator $Iterator;
    public A $A;
    public B $B;
}

function invalid(Test $test, string $prop1, string $prop2, $value) {
    try {
        $test->$prop2 = $value;
        $test->$prop1 =& $test->$prop2;
        echo "Invalid assignment $prop1 =& $prop2 did not error\n";
    } catch (TypeError $e) {}
    try {
        $test->$prop1 = $value;
        $test->$prop2 =& $test->$prop1;
        echo "Invalid assignment $prop2 =& $prop1 did not error\n";
    } catch (TypeError $e) {}
}

function valid(Test $test, string $prop1, string $prop2, $value) {
    try {
        $test->$prop2 = $value;
        $test->$prop1 =& $test->$prop2;
    } catch (TypeError $e) {
        echo "Valid assignment $prop1 =& $prop2 threw {$e->getMessage()}\n";
    }
    try {
        $test->$prop1 = $value;
        $test->$prop2 =& $test->$prop1;
    } catch (TypeError $e) {
        echo "Valid assignment $prop2 =& $prop1 threw {$e->getMessage()}\n";
    }
}

$test = new Test;
invalid($test, 'int', 'float', 42.0);
valid($test, 'int', 'nint', 42);
invalid($test, 'int', 'nint', null);
valid($test, 'nint', 'nstring', null);
invalid($test, 'nint', 'nstring', '42');
valid($test, 'A', 'A', new A);
valid($test, 'A', 'B', new B);
invalid($test, 'A', 'B', new A);
valid($test, 'iterable', 'array', [1, 2, 3]);
valid($test, 'A', 'object', new A);
invalid($test, 'A', 'object', new Test);
valid($test, 'iterable', 'Iterator', new ArrayIterator);
invalid($test, 'Iterator', 'iterable', [1, 2, 3]);
valid($test, 'object', 'iterable', new ArrayIterator);
invalid($test, 'iterable', 'object', new stdClass);

echo "Done\n";

?>
--EXPECT--
Done
