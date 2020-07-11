--TEST--
Bug #74954 (crash after update of generator yielding from finished generator)
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
        $g->current();
}

do {
        foreach ($gens as $i => $g) {
                $g->next();
        }
} while($gens[0]->valid());

?>
--EXPECTF--
Caught exception!
Exception in %s:%d
Stack trace:
#0 %s(%d): from()
#1 [internal function]: gen(Object(Generator))
#2 %s(%d): Generator->next()
#3 {main}
Caught exception!
ClosedGeneratorException: Generator yielded from aborted, no return value available in %s:%d
Stack trace:
#0 [internal function]: gen(Object(Generator))
#1 %s(%d): Generator->next()
#2 {main}
