--TEST--
Multi-parameter function to single-parameter function.
--FILE--
<?php

function volume(int $x, int $y, int $z): int {
    return $x * $y * $z;
}

$p1 = volume(3, 5, ?);
$p2 = volume(3, ?, 9);
$p3 = volume(?, 5, 9);

print $p1(9);
print $p2(5);
print $p3(3);

--EXPECTF--
135
135
135
