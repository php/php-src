--TEST--
Test array_sum() function with objects castable to numeric type
--EXTENSIONS--
gmp
--FILE--
<?php
$input = [gmp_init(25), gmp_init(6)];
var_dump(array_sum($input));
?>
--EXPECT--
int(31)
