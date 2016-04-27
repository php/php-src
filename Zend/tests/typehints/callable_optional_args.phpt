--TEST--
It is possible to pass function that expects more parameters if they are optional and don't have typehints
--FILE--
<?php

function foo1(callable() $cb) {
    $cb();
};
foo1(function (...$a) { var_dump(1); });

function foo2(callable() $cb) {
    $cb();
};
foo2(function ($a = 123) { var_dump(2); });

function foo3(callable(int $a) $cb) {
    $cb(3);
};
foo3(function (int $a, $b = 0) { var_dump($a + $b); });

?>
--EXPECT--
int(1)
int(2)
int(3)
