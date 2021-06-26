--TEST--
Function evaluation order
--FILE--
<?php

function _test1(int $a): int {
    print __FUNCTION__ . PHP_EOL;
    return $a;
}

function _test2(): callable {
    print __FUNCTION__ . PHP_EOL;
    return '\_test3';
}

function _test3(int $a): int {
    print __FUNCTION__ . PHP_EOL;
    return $a;
}

function _test4(int $a): int {
    print __FUNCTION__ . PHP_EOL;
    return $a;
}

$res1 = 5 |> '_test1' |> _test2() |> '_test4';

var_dump($res1);
?>
--EXPECT--
_test1
_test2
_test3
_test4
int(5)
