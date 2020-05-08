--TEST--
Inlined partials.
--FILE--
<?php

function volume(int $x, int $y, int $z): int {
    return $x * $y * $z;
}

$arr = [1, 2, 3];

function mul(float $x, float $y) {
    return $x * $y;
}

$result = array_map(mul(2, ?), $arr);

print_r($result);

--EXPECTF--
Array
(
    [0] => 2
    [1] => 4
    [2] => 6
)
