--TEST--
Testing floatval() and its alias doubleval() Functions
--FILE--
<?php
/* Prototype: float floatval( mixed $var );
 * Description: Returns the float value of var.
 */

echo "*** Testing floatval() with valid float values ***\n";
// different valid  float values 
$valid_floats = array(
                0.0,
                1.0,
                -1.0,
                1.234,
 	        -1.234,
                1.2e3,
	        -1.2e3,
		10.0000000000000000005,
		10.5e+5,
                1e5,
	        -1e5,
                1e-5,
		-1e-1,
		1e+5,
		-1e+5,
		1E5,
		-1E5,
		1E+5,
		-1E+5,
		.5e+7,
		-.5e+7
);

/* loop to check that floatval() recognizes different 
   float values, expected output:float value for valid floating point number */

foreach ($valid_floats as $value ) {
   var_dump( floatval($value) );
}


echo "\n*** Testing doubleval() with valid float values ***\n";
/* loop to check that doubleval() also recognizes different 
   float values, expected output:float value for valid floating point number */

foreach ($valid_floats as $value ) {
   var_dump( doubleval($value) );
}


echo "\n*** Testing floatval() on non floating types ***\n";

// get a resource type variable
$fp = fopen (__FILE__, "r");
fclose($fp);
$dfp = opendir ( dirname(__FILE__) );
closedir($dfp);

// other types in an array 
$not_float_types = array (
                   -2147483648, // max negative integer value
                   2147483648,  // max positive integer value
                   $fp,  // resource
                   $dfp,
                   "0.0", // string
                   "1.0",
	           "-1.3e3",
		   "bob-1.3e3",
                   "10 Some dollars",
	           "10.2 Some Dollars",
	       	   "10.0 dollar" + 1,
		   "10.0 dollar" + 1.0,
                   "",
                   true,
                   NULL,
                   null,
                 );
/* loop through the $not_float_types to see working of 
   floatval() on non float types, expected output: float value valid floating point numbers */
foreach ($not_float_types as $type ) {
   var_dump( floatval($type) );
}


echo "\n*** Testing doubleval() on non floating types ***\n";

/* loop through the $not_float_types to see working of 
   doubleval() on non float types, expected output: float value valid floating point numbers */
foreach ($not_float_types as $type ) {
   var_dump( doubleval($type) );
}




echo "\n*** Testing error conditions ***\n";
//Zero argument
var_dump( floatval() );
var_dump( doubleval() );

//arguments more than expected 
var_dump( floatval(TRUE, FALSE) );
var_dump( doubleval(TRUE, FALSE) );
 
echo "\nDone\n";


?>
--EXPECTF--
*** Testing floatval() with valid float values ***
float(0)
float(1)
float(-1)
float(1.234)
float(-1.234)
float(1200)
float(-1200)
float(10)
float(1050000)
float(100000)
float(-100000)
float(1.0E-5)
float(-0.1)
float(100000)
float(-100000)
float(100000)
float(-100000)
float(100000)
float(-100000)
float(5000000)
float(-5000000)

*** Testing doubleval() with valid float values ***
float(0)
float(1)
float(-1)
float(1.234)
float(-1.234)
float(1200)
float(-1200)
float(10)
float(1050000)
float(100000)
float(-100000)
float(1.0E-5)
float(-0.1)
float(100000)
float(-100000)
float(100000)
float(-100000)
float(100000)
float(-100000)
float(5000000)
float(-5000000)

*** Testing floatval() on non floating types ***

Notice: A non well formed numeric value encountered in %s on line 69

Notice: A non well formed numeric value encountered in %s on line 70
float(-2147483648)
float(2147483648)
float(%d)
float(%d)
float(0)
float(1)
float(-1300)
float(0)
float(10)
float(10.2)
float(11)
float(11)
float(0)
float(1)
float(0)
float(0)

*** Testing doubleval() on non floating types ***
float(-2147483648)
float(2147483648)
float(%d)
float(%d)
float(0)
float(1)
float(-1300)
float(0)
float(10)
float(10.2)
float(11)
float(11)
float(0)
float(1)
float(0)
float(0)

*** Testing error conditions ***

Warning: floatval() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: doubleval() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: floatval() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: doubleval() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Done
