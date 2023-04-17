--TEST--
Test array_product() function with objects castable to numeric type
--EXTENSIONS--
gmp
--FILE--
<?php
$input = [gmp_init(25), gmp_init(6)];

echo "array_product() version:\n";
var_dump(array_product($input));

echo "array_reduce() version:\n";
var_dump(array_reduce($input, fn($carry, $value) => $carry * $value, 1));
?>
--EXPECT--
array_product() version:
int(150)
array_reduce() version:
object(GMP)#5 (1) {
  ["num"]=>
  string(3) "150"
}
