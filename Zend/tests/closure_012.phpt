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
Warning: Undefined variable $i (this will become an error in PHP 9.0) in %s on line %d

Warning: Undefined variable $i (this will become an error in PHP 9.0) in %s on line %d
NULL
int(2)
