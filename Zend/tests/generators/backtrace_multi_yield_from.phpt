--TEST--
Generator backtrace with multi yield from
--FILE--
<?php

function gen() {
    yield 1;
    debug_print_backtrace();
    yield 2;
}

function from($gen) {
    yield from $gen;
}

$gen1 = gen();
$gen2 = from($gen1);
$gen3 = from($gen2);
var_dump($gen3->current());
$gen2->next();
var_dump($gen2->current());
$gen2->next();
var_dump($gen2->current());

?>
--EXPECTF--
int(1)
int(1)
#0  gen() called at [%s:10]
#1  from(Generator Object ())
#2  Generator->next() called at [%s:19]
int(2)
