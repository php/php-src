--TEST--
Check min() optimisation for int and float types
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only"); ?>
--FILE--
<?php

echo "Start as int optimisation:\n";
var_dump(min(10, 5, 3, 2));
var_dump(min(2, 3, 5, 10));
var_dump(min(10, 5, 3.5, 2));
var_dump(min(2, 3.5, 5, 10));
var_dump(min(10, 5, "3", 2));
var_dump(min(2, "3", 5, 10));
var_dump(min(2, 3, "1", 10));
echo "Check that int not representable as float works:\n";
var_dump(min(PHP_INT_MAX-1, PHP_INT_MAX, PHP_INT_MAX*2));
var_dump(min(PHP_INT_MIN+1, PHP_INT_MIN, PHP_INT_MIN*2));
// Has INF
var_dump(min(PHP_INT_MAX-1, PHP_INT_MAX, PHP_INT_MAX**20));

echo "Start as float optimisation:\n";
var_dump(min(10.5, 5.5, 3.5, 2.5));
var_dump(min(2.5, 3.5, 5.5, 10.5));
var_dump(min(10.5, 5.5, 3, 2.5));
var_dump(min(2.5, 3, 5.5, 10.5));
var_dump(min(10.5, 5.5, "3.5", 2.5));
var_dump(min(2.5, "3.5", 5.5, 10.5));
var_dump(min(2.5, 3.5, "1.5", 10.5));
echo "Check that int not representable as float works:\n";
var_dump(min(PHP_INT_MAX*2, PHP_INT_MAX, PHP_INT_MAX-1));
var_dump(min(PHP_INT_MIN*2, PHP_INT_MIN, PHP_INT_MIN+1));
// Has INF
var_dump(min(PHP_INT_MAX**20, PHP_INT_MAX, PHP_INT_MAX-1));

?>
--EXPECT--
Start as int optimisation:
int(2)
int(2)
int(2)
int(2)
int(2)
int(2)
string(1) "1"
Check that int not representable as float works:
int(9223372036854775806)
float(-1.8446744073709552E+19)
int(9223372036854775806)
Start as float optimisation:
float(2.5)
float(2.5)
float(2.5)
float(2.5)
float(2.5)
float(2.5)
string(3) "1.5"
Check that int not representable as float works:
int(9223372036854775806)
float(-1.8446744073709552E+19)
int(9223372036854775806)
