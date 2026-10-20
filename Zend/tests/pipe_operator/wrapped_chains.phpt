--TEST--
Pipe operator chains saved as a closure
--FILE--
<?php

function _test1(int $a): int {
    return $a + 1;
}

function _test2(int $a): int {
    return $a * 2;
}

$func = fn($x) => $x |> '_test1' |> '_test2';

$res1 = $func(5);

var_dump($res1);
?>
--EXPECT--
int(12)
