--TEST--
Printing the stack trace in a generator
--FILE--
<?php

function f1() {
    debug_print_backtrace();
}

function f2($arg1, $arg2) {
    f1();
    yield; // force generator
}

function f3($gen) {
    $gen->rewind(); // trigger run
}

$gen = f2('foo', 'bar');
f3($gen);

?>
--EXPECTF--
#0  f1() called at [%s:%d]
#1  f2(foo, bar)
#2  Generator->rewind() called at [%s:%d]
#3  f3(Generator Object ()) called at [%s:%d]
