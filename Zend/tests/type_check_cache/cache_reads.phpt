--TEST--
Test if type checks work when TCC is warmed up
--FILE--
<?php

class A {}
class B {
    public A $prop;
}

function test1(A $arg) {
    return $arg;
}

function test2($arg): A {
    return $arg;
}

foreach ([1,2] as $iteration) {
    echo "Passing valid object:\n";
    test1(new A);
    echo "Passing invalid object:\n";
    try {
        test1(new B);
    } catch (TypeError $e) {
        echo "TypeError\n";
    }

    echo "Returning valid object:\n";
    test2(new A);
    echo "Returning invalid object:\n";
    try {
        test2(new B);
    } catch (TypeError $e) {
        echo "TypeError\n";
    }

    echo "Writing valid property value\n";
    $b = new B;
    $b->prop = new A;
    echo "Writing invalid property value\n";
    try {
        $b->prop = new B;
    } catch (TypeError $e) {
        echo "TypeError\n";
    }
}

echo "Done\n";
?>
--EXPECTF--
Passing valid object:
Passing invalid object:
TypeError
Returning valid object:
Returning invalid object:
TypeError
Writing valid property value
Writing invalid property value
TypeError
Passing valid object:
Passing invalid object:
TypeError
Returning valid object:
Returning invalid object:
TypeError
Writing valid property value
Writing invalid property value
TypeError
Done
