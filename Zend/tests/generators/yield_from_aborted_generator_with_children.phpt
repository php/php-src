--TEST--
Impossible to yield from a generator which already failed, nested version
--FILE--
<?php

function from() {
    yield 0;
    throw new Exception();
}
function gen($gen) {
    yield from $gen;
}

$gen1 = from();
$gen2 = gen($gen1);
$gen3 = gen($gen1);
try {
    $gen2->next();
} catch (Exception $e) {
    unset($gen2);
}
$gen3->next();

?>
--EXPECTF--
Fatal error: Uncaught Error: Generator passed to yield from was aborted without proper return and is unable to continue in %s:%d
Stack trace:
#0 [internal function]: gen(Object(Generator))
#1 %s(%d): Generator->next()
#2 {main}
  thrown in %s on line %d
