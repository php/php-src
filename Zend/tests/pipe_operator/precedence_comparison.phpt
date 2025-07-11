--TEST--
Pipe binds higher than comparison
--FILE--
<?php

function _test1(int $a): int {
    return $a * 2;
}

// The pipe should run before the comparison.
$res1 = 5 |> _test1(...) == 10 ;
var_dump($res1);

?>
--EXPECTF--
bool(true)
