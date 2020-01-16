--TEST--
Test if type checks work when TCC has no available memory
--INI--
tcc_memory_limit=0
opcache.enable=1
opcache.enable_cli=1
opcache.jit_buffer_size=1M
opcache.jit=1205
--SKIPIF--
<?php require_once('skipif.inc'); ?>
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
