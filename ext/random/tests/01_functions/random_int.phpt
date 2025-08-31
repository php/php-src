--TEST--
Test normal operation of random_int()
--FILE--
<?php

var_dump(is_int(random_int(10, 100)));

$x = random_int(10, 100);
var_dump($x >= 10 && $x <= 100);

var_dump(random_int(-1000, -1) < 0);
var_dump(random_int(-1, PHP_INT_MAX) >= -1);
var_dump(is_int(random_int(PHP_INT_MIN, PHP_INT_MAX)));

var_dump(random_int(42,42));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
int(42)
