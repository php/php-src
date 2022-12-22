--TEST--
Test array_product() function with objects castable to numeric type
--EXTENSIONS--
gmp
--FILE--
<?php
$input = [gmp_init(25), gmp_init(6)];
var_dump(array_product($input));
?>
--EXPECT--
int(1)
