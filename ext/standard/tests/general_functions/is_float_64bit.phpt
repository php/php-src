--TEST--
Test is_float() & its is_double() alias
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--INI--
precision=14
--FILE--
<?php
echo "*** Testing is_float(), is_double() with float values***\n";
// different valid  float values
$floats = array(
  -2147483649, // float value
  2147483648,  // float value
  -0x80000001, // float value, beyond max negative int
  0x800000001, // float value, beyond max positive int
  020000000001, // float value, beyond max positive int
  -020000000001, // float value, beyond max negative int
  0.0,
  -0.1,
  10.0000000000000000005,
  10.5e+5,
  1e5,
  -1e5,
  1e-5,
  -1e-5,
  1e+5,
  -1e+5,
  1E5,
  -1E5,
  1E+5,
  -1E+5,
  1E-5,
  -1E-5,
  .5e+7,
  -.5e+7,
  .6e-19,
  -.6e-19,
  .05E+44,
  -.05E+44,
  .0034E-30,
  -.0034E-30
);
/* loop to check that is_float(), is_double() recognizes
   different float values, expected: bool(true)  */
$loop_counter = 1;
foreach ($floats as $float ) {
  echo "-- Iteration $loop_counter --\n"; $loop_counter++;
  var_dump( is_float($float) );
  var_dump( is_double($float) );
}

echo "\n*** Testing is_float(), is_double() with non float values ***\n";
// get a resource type variable
$fp = fopen (__FILE__, "r");
$dfp = opendir ( __DIR__ );

// unset variable
$unset_var = 10;
unset ($unset_var);

// non_scalar values, objects, arrays, resources and boolean
class foo
{
  var $array = array(10.5);
};
$object = new foo();

$not_floats = array (
  new foo, //object
  $object,

  $fp,  // resource
  $dfp,

  array(),  // arrays
  array(NULL),
  array(0.5e10),
  array(1,2,3,4),
  array("string"),

  NULL,  // nulls
  null,

  true,  // boolean
  TRUE,
  false,
  FALSE,

  "",  // strings
  '',
  "0",
  '0',
  "0.0",
  '0.0',
  '0.5',
  "-0.5",
  "1e5",
  '1e5',
  '1.5e6_string',
  "1.5e6_string",

  1,  // integers, hex and octal
  -1,
  0,
  12345,
  0xFF55,
  -0x673,
  0123,
  -0123,

  @$unset_var,  // unset variable
  @$undefined_var
);
/* loop through the $not_floats to see working of
   is_float(), is_double() on objects,
    arrays, boolean and others */
$loop_counter = 1;
foreach ($not_floats as $value ) {
  echo "--Iteration $loop_counter--\n"; $loop_counter++;
  var_dump( is_float($value) );
  var_dump( is_double($value) );
}

echo "Done\n";
?>
--EXPECT--
*** Testing is_float(), is_double() with float values***
-- Iteration 1 --
bool(false)
bool(false)
-- Iteration 2 --
bool(false)
bool(false)
-- Iteration 3 --
bool(false)
bool(false)
-- Iteration 4 --
bool(false)
bool(false)
-- Iteration 5 --
bool(false)
bool(false)
-- Iteration 6 --
bool(false)
bool(false)
-- Iteration 7 --
bool(true)
bool(true)
-- Iteration 8 --
bool(true)
bool(true)
-- Iteration 9 --
bool(true)
bool(true)
-- Iteration 10 --
bool(true)
bool(true)
-- Iteration 11 --
bool(true)
bool(true)
-- Iteration 12 --
bool(true)
bool(true)
-- Iteration 13 --
bool(true)
bool(true)
-- Iteration 14 --
bool(true)
bool(true)
-- Iteration 15 --
bool(true)
bool(true)
-- Iteration 16 --
bool(true)
bool(true)
-- Iteration 17 --
bool(true)
bool(true)
-- Iteration 18 --
bool(true)
bool(true)
-- Iteration 19 --
bool(true)
bool(true)
-- Iteration 20 --
bool(true)
bool(true)
-- Iteration 21 --
bool(true)
bool(true)
-- Iteration 22 --
bool(true)
bool(true)
-- Iteration 23 --
bool(true)
bool(true)
-- Iteration 24 --
bool(true)
bool(true)
-- Iteration 25 --
bool(true)
bool(true)
-- Iteration 26 --
bool(true)
bool(true)
-- Iteration 27 --
bool(true)
bool(true)
-- Iteration 28 --
bool(true)
bool(true)
-- Iteration 29 --
bool(true)
bool(true)
-- Iteration 30 --
bool(true)
bool(true)

*** Testing is_float(), is_double() with non float values ***
--Iteration 1--
bool(false)
bool(false)
--Iteration 2--
bool(false)
bool(false)
--Iteration 3--
bool(false)
bool(false)
--Iteration 4--
bool(false)
bool(false)
--Iteration 5--
bool(false)
bool(false)
--Iteration 6--
bool(false)
bool(false)
--Iteration 7--
bool(false)
bool(false)
--Iteration 8--
bool(false)
bool(false)
--Iteration 9--
bool(false)
bool(false)
--Iteration 10--
bool(false)
bool(false)
--Iteration 11--
bool(false)
bool(false)
--Iteration 12--
bool(false)
bool(false)
--Iteration 13--
bool(false)
bool(false)
--Iteration 14--
bool(false)
bool(false)
--Iteration 15--
bool(false)
bool(false)
--Iteration 16--
bool(false)
bool(false)
--Iteration 17--
bool(false)
bool(false)
--Iteration 18--
bool(false)
bool(false)
--Iteration 19--
bool(false)
bool(false)
--Iteration 20--
bool(false)
bool(false)
--Iteration 21--
bool(false)
bool(false)
--Iteration 22--
bool(false)
bool(false)
--Iteration 23--
bool(false)
bool(false)
--Iteration 24--
bool(false)
bool(false)
--Iteration 25--
bool(false)
bool(false)
--Iteration 26--
bool(false)
bool(false)
--Iteration 27--
bool(false)
bool(false)
--Iteration 28--
bool(false)
bool(false)
--Iteration 29--
bool(false)
bool(false)
--Iteration 30--
bool(false)
bool(false)
--Iteration 31--
bool(false)
bool(false)
--Iteration 32--
bool(false)
bool(false)
--Iteration 33--
bool(false)
bool(false)
--Iteration 34--
bool(false)
bool(false)
--Iteration 35--
bool(false)
bool(false)
--Iteration 36--
bool(false)
bool(false)
--Iteration 37--
bool(false)
bool(false)
Done
