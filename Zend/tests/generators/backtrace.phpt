--TEST--
Printing the stack trace in a generator
--FILE--
<?php

function f1() {
    debug_print_backtrace();
}

function *f2() {
    f1();
}

function f3($gen) {
    $gen->rewind(); // trigger run
}

$gen = f2();
f3($gen);

?>
--EXPECTF--
#0  f1() called at [%s:%d]
#1  Generator->rewind() called at [%s:%d]
#2  f3(Generator Object ()) called at [%s:%d]
