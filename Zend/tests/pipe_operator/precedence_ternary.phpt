--TEST--
Pipe binds lower than ternary
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

// $config is null, so the second function gets used.
$config = null;
$res1 = 5 |> $config ? _test1(...) : _test2(...);
var_dump($res1);

// $config is truthy, so the ternary binds first
// and evaluates to the first function.
$config = _test3(...);
$res2 = 5 |> $config ? _test1(...) : _test2(...);
var_dump($res2);

// Binding the ternary first doesn't make logical sense,
// so the pipe runs first in this case.
$x = true;
$y = 'beep';
$z = 'default';
$ret3 = $x ? $y |> strlen(...) : $z;
var_dump($ret3);


?>
--EXPECT--
int(10)
int(6)
int(4)
