--TEST--
Bug #70965 (yield from with a common iterator primes too much)
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
--EXPECT--
int(1)
int(2)
int(3)
int(4)
int(5)
int(5)
