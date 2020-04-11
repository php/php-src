--TEST--
Pipe binds lower than coalesce
--FILE--
<?php

function _test1(int $a): int {
    return $a * 2;
}

$bad_func = null;

$res1 = 5 |> $bad_func ?? '_test1';

var_dump($res1);
?>
--EXPECT--
int(10)
