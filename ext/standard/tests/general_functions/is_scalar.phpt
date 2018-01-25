--TEST--
Test is_scalar() function
--FILE--
<?php
/* Prototype: bool is_scalar ( mixed $var );
 * Description: Finds whether a variable is a scalar (i.e integer, float, string or boolean)
 */

echo "*** Testing basic operations ***\n";
$scalar_variables = array(
  0,  // integers
  1,
  -45678,
  0x5FF,  // hexadecimal as integer
  0X566,
  -0xAAF,
  -0XCCF,
  01234,  // octal as integer
  -0126,

  0.0,  // floats
  -1.0,
  1e5,
  -1e7,
  1.6E7,
  475.e-8,
  784.e+30,
  98.45E+40,
  .5E-40,

  "",  // strings
  '',
  " ",
  ' ',
  "string",
  'string',
  "0",  // numeric as string
  "40",
  "50.696",
  "0x534",
  "0X534",

  TRUE,  // boolean
  FALSE,
  true,
  false
);
/* loop through each valid scalar variables in $scalar_variables
   and see the working of is_scalar(), expected output: bool(true)
*/
$loop_counter = 1;
foreach($scalar_variables as $scalar) {
  echo "-- Iteration $loop_counter --\n"; $loop_counter++;
  var_dump( is_scalar($scalar) );
}

echo "\n*** Testing possible variations ***\n";
// different scalar variables which are unset
$int_var = 10;
$float_var = 1e5;
$string_var = "string";
$boolean_var = true;
$object = new stdclass;
$array = array(10);
$resource = opendir('.');
unset($int_var, $float_var, $string_var, $boolean_var, $object, $array, $resource);

// resources
$fp = fopen(__FILE__, "r");
$dfp = opendir(".");

$variation_array = array(
  NULL,
  null,

  array(),  // arrays
  array(NULL),
  array(true),
  array(0),
  array(1,2,3,4),

  $fp,  // resources
  $dfp,

  new stdclass, // object

  @$int_var,  // scalars that are unset
  @$float_var,
  @$string_var,
  @$boolean_var,

  @$array,   // non scalars that are unset
  @$object,
  @$resource,

  @$undefined_var  // undefined variable
);

/* loop through each element of $variation_array to see the
   working of is_scalar on non-scalar values, expected output: bool(false)
*/
$loop_counter = 1;
foreach( $variation_array as $value ) {
  echo "-- Iteration $loop_counter --\n"; $loop_counter++;
  var_dump( is_scalar($value) );
}

echo "\n*** Testing error conditions ***\n";
// Zero arguments
var_dump( is_scalar() );

// Arguments more than expected
var_dump( is_scalar( $scalar_variables[2], $scalar_variables[2]) );
var_dump( is_scalar( new stdclass, new stdclass) );

echo "Done\n";

// close the resources used
fclose($fp);
closedir($dfp);

?>
--EXPECTF--
*** Testing basic operations ***
-- Iteration 1 --
bool(true)
-- Iteration 2 --
bool(true)
-- Iteration 3 --
bool(true)
-- Iteration 4 --
bool(true)
-- Iteration 5 --
bool(true)
-- Iteration 6 --
bool(true)
-- Iteration 7 --
bool(true)
-- Iteration 8 --
bool(true)
-- Iteration 9 --
bool(true)
-- Iteration 10 --
bool(true)
-- Iteration 11 --
bool(true)
-- Iteration 12 --
bool(true)
-- Iteration 13 --
bool(true)
-- Iteration 14 --
bool(true)
-- Iteration 15 --
bool(true)
-- Iteration 16 --
bool(true)
-- Iteration 17 --
bool(true)
-- Iteration 18 --
bool(true)
-- Iteration 19 --
bool(true)
-- Iteration 20 --
bool(true)
-- Iteration 21 --
bool(true)
-- Iteration 22 --
bool(true)
-- Iteration 23 --
bool(true)
-- Iteration 24 --
bool(true)
-- Iteration 25 --
bool(true)
-- Iteration 26 --
bool(true)
-- Iteration 27 --
bool(true)
-- Iteration 28 --
bool(true)
-- Iteration 29 --
bool(true)
-- Iteration 30 --
bool(true)
-- Iteration 31 --
bool(true)
-- Iteration 32 --
bool(true)
-- Iteration 33 --
bool(true)

*** Testing possible variations ***
-- Iteration 1 --
bool(false)
-- Iteration 2 --
bool(false)
-- Iteration 3 --
bool(false)
-- Iteration 4 --
bool(false)
-- Iteration 5 --
bool(false)
-- Iteration 6 --
bool(false)
-- Iteration 7 --
bool(false)
-- Iteration 8 --
bool(false)
-- Iteration 9 --
bool(false)
-- Iteration 10 --
bool(false)
-- Iteration 11 --
bool(false)
-- Iteration 12 --
bool(false)
-- Iteration 13 --
bool(false)
-- Iteration 14 --
bool(false)
-- Iteration 15 --
bool(false)
-- Iteration 16 --
bool(false)
-- Iteration 17 --
bool(false)
-- Iteration 18 --
bool(false)

*** Testing error conditions ***

Warning: is_scalar() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: is_scalar() expects exactly 1 parameter, 2 given in %s on line %d
NULL

Warning: is_scalar() expects exactly 1 parameter, 2 given in %s on line %d
NULL
Done
