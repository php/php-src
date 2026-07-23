--TEST--
Partial application must not leak bound arguments when construction throws
--FILE--
<?php
class Obj {}

function typed(int $a, $b, $c) {}
try {
    typed([1, 2, 3], new Obj(), ?);
} catch (\TypeError $e) {
    echo $e::class, "\n";
}

function typed2($a, int $b, $c, $d) {}
try {
    typed2(new Obj(), [1, 2], new Obj(), ?);
} catch (\TypeError $e) {
    echo $e::class, "\n";
}

function one($a) {}
try {
    one(new Obj(), new Obj(), ?);
} catch (\ArgumentCountError $e) {
    echo $e::class, "\n";
}

echo "OK\n";
?>
--EXPECT--
TypeError
TypeError
ArgumentCountError
OK
