--TEST--
GH-19493 002: Var not stored before YIELD_FROM
--FILE--
<?php

function f() {
    $offset = 0;
    yield from [true];
    for ($i = 0; $i < 100; $i++) {
        $offset++;
        if ($offset === 99) {
            break;
        }
        yield from [true];
    }
    return $offset;
}
$gen = f();
foreach ($gen as $v) {}
var_dump($gen->getReturn());

?>
--EXPECT--
int(99)
