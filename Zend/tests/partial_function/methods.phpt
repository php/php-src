--TEST--
Methods.
--FILE--
<?php

class Action {
    function volume(int $x, int $y, int $z): int {
        return $x * $y * $z;
    }
}

$a = new Action();

$p1 = $a->volume(3, 5, ?);
$p2 = $a->volume(3, ?, 9);
$p3 = $a->volume(?, 5, 9);

print $p1(9);
print $p2(5);
print $p3(3);

--EXPECTF--
135
135
135
