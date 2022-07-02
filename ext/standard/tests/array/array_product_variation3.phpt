--TEST--
Test array_product() function : variation
--FILE--
<?php
echo "*** Testing array_product() : variations - negative numbers***\n";

echo "\n-- Testing array_product() function with one negative number --\n";
var_dump( array_product(array(-2)) );

echo "\n-- Testing array_product() function with two negative numbers --\n";
var_dump( array_product(array(-2, -3)) );

echo "\n-- Testing array_product() function with three negative numbers --\n";
var_dump( array_product(array(-2, -3, -4)) );

echo "\n-- Testing array_product() function with negative floats --\n";
var_dump( array_product(array(-1.5)));

echo "\n-- Testing array_product() function with negative floats --\n";
var_dump( array_product(array(-99999999.9, 99999999.1)));


?>
--EXPECT--
*** Testing array_product() : variations - negative numbers***

-- Testing array_product() function with one negative number --
int(-2)

-- Testing array_product() function with two negative numbers --
int(6)

-- Testing array_product() function with three negative numbers --
int(-24)

-- Testing array_product() function with negative floats --
float(-1.5)

-- Testing array_product() function with negative floats --
float(-9999999900000000)
