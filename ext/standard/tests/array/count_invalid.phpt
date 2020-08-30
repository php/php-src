--TEST--
Only arrays and countable objects can be counted
--FILE--
<?php

$result = count(null);
var_dump($result);

$result = count("string");
var_dump($result);

$result = count(123);
var_dump($result);

$result = count(true);
var_dump($result);

$result = count(false);
var_dump($result);

$result = count((object) []);
var_dump($result);

?>
--EXPECTF--
Warning: count(): Argument #1 ($var) must be of type Countable|array, null given in %s on line %d
int(0)

Warning: count(): Argument #1 ($var) must be of type Countable|array, string given in %s on line %d
int(1)

Warning: count(): Argument #1 ($var) must be of type Countable|array, int given in %s on line %d
int(1)

Warning: count(): Argument #1 ($var) must be of type Countable|array, bool given in %s on line %d
int(1)

Warning: count(): Argument #1 ($var) must be of type Countable|array, bool given in %s on line %d
int(1)

Warning: count(): Argument #1 ($var) must be of type Countable|array, stdClass given in %s on line %d
int(1)
