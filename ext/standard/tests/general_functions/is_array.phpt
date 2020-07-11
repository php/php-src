--TEST--
Test is_array() function
--FILE--
<?php
echo "*** Testing is_array() on different type of arrays ***\n";
/* different types of arrays */
$arrays = array(
  array(),
  array(NULL),
  array(null),
  array(true),
  array(""),
  array(''),
  array(array(), array()),
  array(array(1, 2), array('a', 'b')),
  array(1 => 'One'),
  array("test" => "is_array"),
  array(0),
  array(-1),
  array(10.5, 5.6),
  array("string", "test"),
  array('string', 'test')
);
/* loop to check that is_array() recognizes different
   type of arrays, expected output bool(true) */
$loop_counter = 1;
foreach ($arrays as $var_array ) {
  echo "-- Iteration $loop_counter --\n"; $loop_counter++;
  var_dump( is_array ($var_array) );
}

echo "\n*** Testing is_array() on non array types ***\n";

// get a resource type variable
$fp = fopen (__FILE__, "r");
$dfp = opendir ( __DIR__ );

// unset variables
$unset_array = array(10);
unset($unset_array);

// other types in a array
$varient_arrays = array (
  /* integers */
  543915,
  -5322,
  0x55F,
  -0xCCF,
  123,
  -0654,

  /* strings */
  "",
  '',
  "0",
  '0',
  'string',
  "string",

  /* floats */
  10.0000000000000000005,
  .5e6,
  -.5E7,
  .5E+8,
  -.5e+90,
  1e5,

  /* objects */
  new stdclass,

  /* resources */
  $fp,
  $dfp,

  /* nulls */
  null,
  NULL,

  /* boolean */
  true,
  TRUE,
  FALSE,
  false,

  /* unset/undefined arrays  */
  @$unset_array,
  @$undefined_array
);
/* loop through the $varient_array to see working of
   is_array() on non array types, expected output bool(false) */
$loop_counter = 1;
foreach ($varient_arrays as $type ) {
  echo "-- Iteration $loop_counter --\n"; $loop_counter++;
  var_dump( is_array ($type) );
}

echo "Done\n";
/* close resources */
fclose($fp);
closedir($dfp);
?>
--EXPECT--
*** Testing is_array() on different type of arrays ***
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

*** Testing is_array() on non array types ***
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
Done
