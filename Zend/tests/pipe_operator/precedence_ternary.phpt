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

$bad_func = null;

$res1 = 5 |> $bad_func ? '_test1' : '_test2';

var_dump($res1);
?>
--EXPECT--
int(10)
