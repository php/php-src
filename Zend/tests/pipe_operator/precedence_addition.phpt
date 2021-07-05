--TEST--
Pipe binds lower than addition
--FILE--
<?php

function _test1(int $a): int {
    return $a + 1;
}

$bad_func = null;

$res1 = 5 + 2 |>  '_test1';

var_dump($res1);
?>
--EXPECT--
int(8)
