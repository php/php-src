--TEST--
Multiple yield from on a same Generator throwing an Exception (no longer supported)
--FILE--
<?php
function from() {
    yield 1;
    throw new Exception();
}

function gen($gen) {
    try {
        var_dump(yield from $gen);
    } catch (Exception $e) { print "Caught exception!\n$e\n"; }
}

$gen = from();
$gens[] = gen($gen);
$gens[] = gen($gen);

foreach ($gens as $g) {
    $g->current(); // init.
}

do {
    foreach ($gens as $i => $g) {
        print "Generator $i\n";
        var_dump($g->current());
        $g->next();
    }
} while($gens[0]->valid());
?>
--EXPECTF--
Fatal error: Uncaught Error: A different generator already yields from this generator in %s:%d
Stack trace:
#0 [internal function]: gen(Object(Generator))
#1 %s(%d): Generator->current()
#2 {main}
  thrown in %s on line %d
