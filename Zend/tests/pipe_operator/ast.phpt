--TEST--
Test that a pipe operator displays as a pipe operator when outputting syntax.
--FILE--
<?php

function _test(int $a): int {
    return $a + 1;
}

function abool(int $x): bool {
    return false;
}

try {
    assert((5 |> '_test') == 99);
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert((5 |> _test(...)) == 99);
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

try {
    assert(5 |> abool(...));
} catch (AssertionError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECTF--
assert((5 |> '_test') == 99)
assert((5 |> _test(...)) == 99)
assert(5 |> abool(...))
