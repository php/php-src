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
#0 %s(10): gen()
#1 [internal function]: from(Object(Generator))
#2 %s(19): Generator->next()
int(2)
