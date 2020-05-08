--TEST--
Nested partials.
--FILE--
<?php

function volume(int $x, int $y, int $z): int {
    return $x * $y * $z;
}

$p1 = volume(3, ?, ?);
$p2 = $p1(?, 9);

print $p1(5);

--EXPECTF--
135
