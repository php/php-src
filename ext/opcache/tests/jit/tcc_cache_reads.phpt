--TEST--
Test if type checks work when TCC is warmed up
--INI--
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
}

echo "Done\n";
?>
--EXPECTF--
Passing valid object:
Passing invalid object:
TypeError
Passing valid object:
Passing invalid object:
TypeError
Done
