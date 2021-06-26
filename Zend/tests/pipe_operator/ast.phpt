--TEST--
Test that a pipe operator displays as a pipe operator when outputting syntax.
--FILE--
<?php

function _test(int $a): int {
    return $a + 1;
}

try {
    assert((5 |> '_test') == 99);
} catch (AssertionError $e) {
    print $e->getMessage();
}

?>
--EXPECTF--
assert(5 |> '_test' == 99)
