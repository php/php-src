--TEST--
Bug #70965 (yield from with a common iterator primes too much) (no longer supported)
--FILE--
<?php

function it() {
    yield from [1, 2, 3, 4, 5];
}

function bar($g) {
    yield from $g;
}

$gen = it();
$gens[] = bar($gen);
$gens[] = bar($gen);

do {
    foreach($gens as $g) {
        var_dump($g->current());
        $gen->next();
    }
} while ($gen->valid());

?>
--EXPECTF--
int(1)

Fatal error: Uncaught Error: A different generator already yields from this generator in %s:%d
Stack trace:
#0 [internal function]: bar(Object(Generator))
#1 %s(%d): Generator->current()
#2 {main}
  thrown in %s on line %d
