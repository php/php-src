--TEST--
Test is_numeric() function
--FILE--
<?php
/* Prototype: bool is_numeric ( mixed $var );
 * Description: Finds whether a variable is a number or a numeric string  
 */

echo "*** Testing is_numeric() with valid numeric values ***\n";
// different valid numeric  vlaues 
$numerics = array(
  0,
  1,
  -1,
  -0,
  +0,
  0.0,
  -0.0,
  +0.0,
  1.0,
  -1.0,
  +1.0,
  .5,
  -.5,
  +.5,
  -.5e-2,
  .5e-2,
  +.5e-2,
  +.5E+2,
  0.70000000,
  +0.70000000,
  -0.70000000,
  1234567890123456,
  -1234567890123456,
  984847472827282718178,
  -984847472827282718178,
  123.56e30,
  123.56E30,
  426.45e-30,
  5657.3E-40,
  3486.36e+40,
  3486.36E+90,
  -3486.36E+10,
  -3486.36e+80,
  -426.45e-50,
  -426.45E-99,
  1e2,
  -1e2,
  -1e-2,
  +1e2,
  +1e+2,
  +1e-2,
  +1e+2,
  2245555555555555.444,
  1.444444444444444444,
  0xff,	// hexa decimal numbers
  0xFF,
  //0x1111111111111111111111,
  -0x1111111,
  +0x6698319,
  01000000000000000000000, 
  0123,
  0345,
  -0200001,  
  -0200001.7,  
  0200001.7,  
  +0200001,  
  +0200001.7,  
  +0200001.7,  
  2.00000000000000000000001, // a float value with more precision points
  "1",	// numeric in the form of string
  "-1",
  "1e2",
  " 1",
  "2974394749328742328432",
  "-1e-2",
  '1',
  '-1',
  '1e2',
  ' 1',
  '2974394749328742328432',
  '-1e-2',
  "0123",
  '0123',
  "-0123",
  "+0123",
  '-0123',
  '+0123'
);
/* loop to check that is_numeric() recognizes different 
   numeric values, expected output: bool(true) */
$loop_counter = 1;
foreach ($numerics as $num ) {
  echo "-- Iteration $loop_counter --\n"; $loop_counter++;
  var_dump( is_numeric($num) );
}

echo "\n*** Testing is_numeric() on non numeric types ***\n";

// get a resource type variable
$fp = fopen (__FILE__, "r");
$dfp = opendir ( dirname(__FILE__) );

// unset variable
$unset_var = 10.5;
unset ($unset_var);

// other types in a array 
$not_numerics = array(
  "0x80001",
  "-0x80001", 
  "+0x80001", 
  "-0x80001.5",
  "0x80001.5", 
  new stdclass, // object
  $fp,  // resource
  $dfp,
  array(),
  array("string"),
  "",
  "1 ",
  "- 1",
  "1.2.4",
  "1e7.6",
  "3FF",
  "20 test",
  "3.6test",
  "1,000",
  "NULL", 
  "true",
  true,
  NULL,
  null,
  TRUE,
  FALSE,
  false,
  @$unset_var, // unset variable
  @$undefined_var
);
/* loop through the $not_numerics to see working of 
   is_numeric() on non numeric values, expected output: bool(false) */
$loop_counter = 1;
foreach ($not_numerics as $type ) {
  echo "-- Iteration $loop_counter --\n"; $loop_counter++;
  var_dump( is_numeric($type) );
}

echo "\n*** Testing error conditions ***\n";
//Zero argument
var_dump( is_numeric() );

//arguments more than expected 
var_dump( is_numeric("10", "20") );
 
echo "Done\n";

// close the resources used
fclose($fp);
closedir($dfp);

?>
--EXPECTF--
*** Testing is_numeric() with valid numeric values ***
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
-- Iteration 34 --
bool(true)
-- Iteration 35 --
bool(true)
-- Iteration 36 --
bool(true)
-- Iteration 37 --
bool(true)
-- Iteration 38 --
bool(true)
-- Iteration 39 --
bool(true)
-- Iteration 40 --
bool(true)
-- Iteration 41 --
bool(true)
-- Iteration 42 --
bool(true)
-- Iteration 43 --
bool(true)
-- Iteration 44 --
bool(true)
-- Iteration 45 --
bool(true)
-- Iteration 46 --
bool(true)
-- Iteration 47 --
bool(true)
-- Iteration 48 --
bool(true)
-- Iteration 49 --
bool(true)
-- Iteration 50 --
bool(true)
-- Iteration 51 --
bool(true)
-- Iteration 52 --
bool(true)
-- Iteration 53 --
bool(true)
-- Iteration 54 --
bool(true)
-- Iteration 55 --
bool(true)
-- Iteration 56 --
bool(true)
-- Iteration 57 --
bool(true)
-- Iteration 58 --
bool(true)
-- Iteration 59 --
bool(true)
-- Iteration 60 --
bool(true)
-- Iteration 61 --
bool(true)
-- Iteration 62 --
bool(true)
-- Iteration 63 --
bool(true)
-- Iteration 64 --
bool(true)
-- Iteration 65 --
bool(true)
-- Iteration 66 --
bool(true)
-- Iteration 67 --
bool(true)
-- Iteration 68 --
bool(true)
-- Iteration 69 --
bool(true)
-- Iteration 70 --
bool(true)
-- Iteration 71 --
bool(true)
-- Iteration 72 --
bool(true)
-- Iteration 73 --
bool(true)
-- Iteration 74 --
bool(true)
-- Iteration 75 --
bool(true)
-- Iteration 76 --
bool(true)

*** Testing is_numeric() on non numeric types ***
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
-- Iteration 19 --
bool(false)
-- Iteration 20 --
bool(false)
-- Iteration 21 --
bool(false)
-- Iteration 22 --
bool(false)
-- Iteration 23 --
bool(false)
-- Iteration 24 --
bool(false)
-- Iteration 25 --
bool(false)
-- Iteration 26 --
bool(false)
-- Iteration 27 --
bool(false)
-- Iteration 28 --
bool(false)
-- Iteration 29 --
bool(false)

*** Testing error conditions ***

Warning: is_numeric() expects exactly 1 parameter, 0 given in %s on line %d
NULL

Warning: is_numeric() expects exactly 1 parameter, 2 given in %s on line %d
NULL
Done
