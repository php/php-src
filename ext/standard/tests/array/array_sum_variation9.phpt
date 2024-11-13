--TEST--
Test array_sum() function with objects castable to numeric type
--EXTENSIONS--
gmp
--FILE--
<?php
$input = [gmp_init(25), gmp_init(6)];

echo "array_sum() version:\n";
var_dump(array_sum($input));

echo "array_reduce() version:\n";
var_dump(array_reduce($input, fn($carry, $value) => $carry + $value, 0));
?>
--EXPECT--
array_sum() version:
int(31)
array_reduce() version:
object(GMP)#5 (1) {
  ["num"]=>
  string(2) "31"
}
