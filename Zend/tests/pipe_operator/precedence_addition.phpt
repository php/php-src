--TEST--
Pipe binds lower than addition
--FILE--
<?php

function _test1(int $a): int {
    return $a * 2;
}

// This should add 5+2 first, then pipe that to _test1.
$res1 = 5 + 2 |>  '_test1';

var_dump($res1);
?>
--EXPECT--
int(14)
