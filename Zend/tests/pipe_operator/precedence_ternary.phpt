--TEST--
Pipe binds higher than ternary
--FILE--
<?php

function _test1(int $a): int {
    return $a + 1;
}

function _test2(int $a): int {
    return $a * 2;
}

function _test3(int $a): int {
    return $a * 100;
}

function is_odd(int $a): bool {
    return (bool)($a % 2);
}

$res1 = 5 |> is_odd(...) ? 'odd' : 'even';
var_dump($res1);

// The pipe binds first, resulting in bool ? int : string, which is well-understood.
$x = true;
$y = 'beep';
$z = 'default';
$ret3 = $x ? $y |> strlen(...) : $z;
var_dump($ret3);


?>
--EXPECT--
string(3) "odd"
int(4)
