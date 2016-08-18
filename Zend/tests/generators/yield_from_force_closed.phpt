--TEST--
Cannot "yield from" from force closed generator
--FILE--
<?php

function gen1() {
    echo "gen1\n";
    yield 1;
}

function gen2() {
    try {
        echo "try\n";
        yield from gen1();
    } finally {
        echo "finally\n";
        yield from gen1();
    }
}

try {
    $gen = gen2();
    $gen->rewind();
    unset($gen);
} catch (Error $e) {
    echo $e, "\n";
}

?>
--EXPECTF--
try
gen1
finally
Error: Cannot use "yield from" in a force-closed generator in %s:%d
Stack trace:
#0 %s(%d): gen2()
#1 {main}
