--TEST--
Test join() function : usage variations - unexpected values for 'glue' argument
--FILE--
<?php
/* Prototype  : string join( string $glue, array $pieces )
 * Description: Join array elements with a string
 * Source code: ext/standard/string.c
 * Alias of function: implode()
*/

/*
 * testing join() by passing different unexpected value for glue argument
*/

echo "*** Testing join() : usage variations ***\n";
// initialize all required variables
$pieces = array("element1", "element2");

// get an unset variable
$unset_var = 'string_val';
unset($unset_var);

// get a resource variable
$fp = fopen(__FILE__, "r");

// define a class
class test
{
   var $t = 10;
   function __toString() {
     return  "testObject";
   }
}

// array with different values
$values =  array (

  // integer values
  0,
  1,
  12345,
  -2345,

  // float values
  10.5,
  -10.5,
  10.1234567e10,
  10.7654321E-10,
  .5,

  // array values
  array(),
  array(0),
  array(1),
  array(1, 2),
  array('color' => 'red', 'item' => 'pen'),

  // boolean values
  true,
  false,
  TRUE,
  FALSE,

  // objects
  new test(),

  // empty string
  "",
  '',

  // null values
  NULL,
  null,

  // resource variable
  $fp,

  // undefined variable
  @$undefined_var,

  // unset variable
  @$unset_var
);


// loop through each element of the array and check the working of join()
// when $glue argument is supplied with different values
echo "\n--- Testing join() by supplying different values for 'glue' argument ---\n";
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  $glue = $values [$index];

  try {
    var_dump(join($glue, $pieces));
  } catch (TypeError $exception) {
    echo $exception->getMessage() . "\n";
  }

  $counter++;
}

echo "Done\n";
?>
--EXPECTF--
*** Testing join() : usage variations ***

--- Testing join() by supplying different values for 'glue' argument ---
-- Iteration 1 --
The first argument must be string
-- Iteration 2 --
The first argument must be string
-- Iteration 3 --
The first argument must be string
-- Iteration 4 --
The first argument must be string
-- Iteration 5 --
The first argument must be string
-- Iteration 6 --
The first argument must be string
-- Iteration 7 --
The first argument must be string
-- Iteration 8 --
The first argument must be string
-- Iteration 9 --
The first argument must be string
-- Iteration 10 --
The first argument must be string
-- Iteration 11 --
The first argument must be string
-- Iteration 12 --
The first argument must be string
-- Iteration 13 --
The first argument must be string
-- Iteration 14 --
The first argument must be string
-- Iteration 15 --
The first argument must be string
-- Iteration 16 --
The first argument must be string
-- Iteration 17 --
The first argument must be string
-- Iteration 18 --
The first argument must be string
-- Iteration 19 --
The first argument must be string
-- Iteration 20 --
string(16) "element1element2"
-- Iteration 21 --
string(16) "element1element2"
-- Iteration 22 --
The first argument must be string
-- Iteration 23 --
The first argument must be string
-- Iteration 24 --
The first argument must be string
-- Iteration 25 --
The first argument must be string
-- Iteration 26 --
The first argument must be string
Done
