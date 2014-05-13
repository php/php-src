--TEST--
Closure 012: Undefined lexical variables
--FILE--
<?php
$lambda = function () use ($i) {
    return ++$i;
};
$lambda();
$lambda();
var_dump($i);
$lambda = function () use (&$i) {
    return ++$i;
};
$lambda();
$lambda();
var_dump($i);
?>
--EXPECTF--
Notice: Undefined variable: i in %s on line 2

Notice: Undefined variable: i in %s on line 7
NULL
int(2)

