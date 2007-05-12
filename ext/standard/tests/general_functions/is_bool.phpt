--TEST--
Test is_bool() function
--FILE--
<?php
/* Prototype: bool is_bool ( mixed $var );
 * Description: Finds whether the given variable is a boolean  
 */

echo "*** Testing is_bool() with valid boolean values ***\n";
// different valid  boolean vlaues 
$valid_bools = array(
  TRUE,
  FALSE,
  true,
  false,
);
/* loop to check that is_bool() recognizes different 
   bool values, expected output: bool(true) */
$loop_counter = 1;
foreach ($valid_bools as $bool_val ) {
  echo "-- Iteration $loop_counter --\n"; $loop_counter++; 
  var_dump( is_bool($bool_val) );
}

echo "\n*** Testing is_bool() on non boolean values ***\n";

// get a resource type variable
$fp = fopen (__FILE__, "r");
$dfp = opendir ( dirname(__FILE__) );

// unset variable
$unset_bool1 = true;
$unset_bool2 = false;
$unset_var = 0;
unset ($unset_bool1);
unset ($unset_bool2);
unset ($unset_var);

// other types in a array 
$not_bool_types = array (
  /* integers */
  0,
  1,
  -1,
  -0,
  543915,
  -5322,
  0x0,
  0x1,
  0x55F,
  -0xCCF,
  0123,
  -0654,
  00,
  01,

  /* strings */
  "",
  '',
  "0",
  '0',
  "1",
  '1',
  'string',
  "string",
  "true",
  "false",
  "FALSE",
  "TRUE",
  'true',
  'false',
  'FALSE',
  'TRUE',
  "NULL",
  "null",

  /* floats */
  0.0,
  1.0,
  -1.0,
  10.0000000000000000005,
  .5e6,
  -.5E7,
  .5E+8,
  -.5e+90,
  1e5,
  -1e5,
  1E5,
  -1E7,

  /* objects */
  new stdclass,

  /* resources */
  $fp,
  $dfp,

  /* nulls */
  null,
  NULL,
  
  /* arrays */
  array(),
  array(0),
  array(1),
  array(NULL),
  array(null),
  array("string"),
  array(true),
  array(TRUE),
  array(false),
  array(FALSE),
  array(1,2,3,4),
  array(1 => "One", "two" => 2),

  /* unset bool vars and undefined var */
  @$unset_bool1, 
  @$unset_bool2, 
  @$unset_var, 
  @$undefined_var
);
/* loop through the $not_bool_types to see working of 
   is_bool() on non bull types, expected output: bool(false) */
$loop_counter = 1;
foreach ($not_bool_types as $type ) {
  echo "-- Iteration $loop_counter --\n"; $loop_counter++; 
  var_dump( is_bool($type) );
}

echo "\n*** Testing error conditions ***\n";
//Zero argument
var_dump( is_bool() );

//arguments more than expected 
var_dump( is_bool(TRUE, FALSE) );
 
echo "Done\n";
?>

--CLEAN--
// close resources
fclose($fp);
closedir($dfp);

--EXPECTF--
*** Testing is_bool() with valid boolean values ***
-- Iteration 1 --
bool(true)
-- Iteration 2 --
bool(true)
-- Iteration 3 --
bool(true)
-- Iteration 4 --
bool(true)

*** Testing is_bool() on non boolean values ***
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
-- Iteration 30 --
bool(false)
-- Iteration 31 --
bool(false)
-- Iteration 32 --
bool(false)
-- Iteration 33 --
bool(false)
-- Iteration 34 --
bool(false)
-- Iteration 35 --
bool(false)
-- Iteration 36 --
bool(false)
-- Iteration 37 --
bool(false)
-- Iteration 38 --
bool(false)
-- Iteration 39 --
bool(false)
-- Iteration 40 --
bool(false)
-- Iteration 41 --
bool(false)
-- Iteration 42 --
bool(false)
-- Iteration 43 --
bool(false)
-- Iteration 44 --
bool(false)
-- Iteration 45 --
bool(false)
-- Iteration 46 --
bool(false)
-- Iteration 47 --
bool(false)
-- Iteration 48 --
bool(false)
-- Iteration 49 --
bool(false)
-- Iteration 50 --
bool(false)
-- Iteration 51 --
bool(false)
-- Iteration 52 --
bool(false)
-- Iteration 53 --
bool(false)
-- Iteration 54 --
bool(false)
-- Iteration 55 --
bool(false)
-- Iteration 56 --
bool(false)
-- Iteration 57 --
bool(false)
-- Iteration 58 --
bool(false)
-- Iteration 59 --
bool(false)
-- Iteration 60 --
bool(false)
-- Iteration 61 --
bool(false)
-- Iteration 62 --
bool(false)
-- Iteration 63 --
bool(false)
-- Iteration 64 --
bool(false)
-- Iteration 65 --
bool(false)

*** Testing error conditions ***

Warning: is_bool(): Only one argument expected in %s on line %d
bool(false)

Warning: is_bool(): Only one argument expected in %s on line %d
bool(false)
Done
