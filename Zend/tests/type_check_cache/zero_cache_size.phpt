--TEST--
Test if type checks work when TCC has no available memory
--INI--
tcc_memory_limit=0
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
