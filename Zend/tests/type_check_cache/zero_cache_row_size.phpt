--TEST--
Test if type checks work when TCC row size is zero
--INI--
tcc_class_slots=0
--FILE--
<?php

class A {}
class B {}

function test(A $arg) {
    is_object($arg);
}

echo "Passing valid object:\n";
test(new A);
echo "Passing invalid object:\n";
try {
    test(new B);
} catch (TypeError $e) {
    echo "TypeError\n";
}

echo "Done\n";
?>
--EXPECTF--
Passing valid object:
Passing invalid object:
TypeError
Done
