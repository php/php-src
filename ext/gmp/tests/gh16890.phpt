--TEST--
GH-16890 (array_sum() with GMP can loose precision (LLP64))
--EXTENSIONS--
gmp
--FILE--
<?php
$large_int_string = (string) (PHP_INT_MAX - 1);
var_dump(array_sum([new GMP($large_int_string), 1]) === PHP_INT_MAX);
?>
--EXPECT--
bool(true)
