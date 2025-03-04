--TEST--
Pipe operator chains
--FILE--
<?php

function _test1(int $a): int {
    return $a + 1;
}

function _test2(int $a): int {
    return $a * 2;
}

$res1 = 5 |> '_test1' |> '_test2';

var_dump($res1);
?>
--EXPECT--
int(12)
