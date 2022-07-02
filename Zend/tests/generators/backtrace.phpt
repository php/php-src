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
#0 %s(%d): f1()
#1 [internal function]: f2('foo', 'bar')
#2 %s(%d): Generator->rewind()
#3 %s(%d): f3(Object(Generator))
