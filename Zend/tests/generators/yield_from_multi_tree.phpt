--TEST--
yield from on multiple trees needing merge (no longer supported)
--FILE--
<?php

function from($levels) {
    foreach (range(0, 2 << $levels) as $v) {
        yield $v;
    }
}

function gen($gen, $level) {
    yield from $gen;
}

foreach (range(0, 6) as $levels) {
    print "$levels level".($levels == 1 ? "" : "s")."\n\n";

    $all = array();
    $all[] = $gens[0][0] = from($levels);

    for ($level = 1; $level < $levels; $level++) {
        for ($i = 0; $i < (1 << $level); $i++) {
            $all[] = $gens[$level][$i] = gen($gens[$level-1][$i >> 1], $level);
        }
    }

    while (1) {
        foreach ($all as $gen) {
            var_dump($gen->current());
            $gen->next();
            if (!$gen->valid()) {
                break 2;
            }
        }
    }

    print "\n\n";
}
?>
--EXPECTF--
0 levels

int(0)
int(1)
int(2)


1 level

int(0)
int(1)
int(2)
int(3)
int(4)


2 levels

int(0)
int(1)

Fatal error: Uncaught Error: A different generator already yields from this generator in %s:%d
Stack trace:
#0 [internal function]: gen(Object(Generator), 1)
#1 %s(%d): Generator->current()
#2 {main}
  thrown in %s on line %d
