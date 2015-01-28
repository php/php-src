--TEST--
Ensure IS_BIGINT and IS_LONG considered same in strict equality
--FILE--
<?php

$long1 = 1;
$bigint1 = (PHP_INT_MAX + 1) - PHP_INT_MAX;

var_dump($long1 === $long1);
var_dump($long1 !== $long1);
var_dump($bigint1 === $bigint1);
var_dump($bigint1 !== $bigint1);
var_dump($bigint1 === $long1);
var_dump($bigint1 !== $long1);
var_dump($long1 === $bigint1);
var_dump($long1 !== $bigint1);
--EXPECT--
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
bool(true)
bool(false)
