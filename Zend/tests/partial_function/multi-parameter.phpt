--TEST--
Multi-parameter function to multi-parameter function.
--FILE--
<?php

function volume(int $x, int $y, int $z): int {
    return $x * $y * $z;
}

$p1 = volume(3, ?, ?);
$p2 = volume(?, ?, 9);
$p3 = volume(?, 5, ?);

print $p1(5, 9);
print $p2(3, 5);
print $p3(3, 9);

--EXPECTF--
135
135
135
