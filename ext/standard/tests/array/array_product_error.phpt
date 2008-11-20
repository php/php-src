--TEST--
Test array_product() function : error conditions 
--FILE--
<?php
/* Prototype  : mixed array_product(array input)
 * Description: Returns the product of the array entries 
 * Source code: ext/standard/array.c
 * Alias to functions: 
 */

echo "*** Testing array_product() : error conditions ***\n";

// Zero arguments
echo "\n-- Testing array_product() function with Zero arguments --\n";
var_dump( array_product() );

//Test array_product with one more than the expected number of arguments
echo "\n-- Testing array_product() function with more than expected no. of arguments --\n";
$input = array(1, 2);
$extra_arg = 10;
var_dump( array_product($input, $extra_arg) );

echo "\n-- Testing array_product() function incorrect argument type --\n";
var_dump( array_product("bob") );

?>
===DONE===
--EXPECTF--
*** Testing array_product() : error conditions ***

-- Testing array_product() function with Zero arguments --

Warning: Wrong parameter count for array_product() in %sarray_product_error.php on line %d
NULL

-- Testing array_product() function with more than expected no. of arguments --

Warning: Wrong parameter count for array_product() in %sarray_product_error.php on line %d
NULL

-- Testing array_product() function incorrect argument type --

Warning: array_product(): The argument should be an array in %sarray_product_error.php on line %d
NULL
===DONE===