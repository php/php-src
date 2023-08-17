--TEST--
Test join() function : usage variations - unexpected values for 'pieces' argument(Bug#42789)
--FILE--
<?php
/*
 * test join() by passing different unexpected value for pieces argument
*/

echo "*** Testing join() : usage variations ***\n";
// initialize all required variables
$glue = '::';

// get an unset variable
$unset_var = array(1, 2);
unset($unset_var);

// get a resource variable
$fp = fopen(__FILE__, "r");

// define a class
class test
{
  var $t = 10;
  var $p = 10;
  function __toString() {
    return "testObject";
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
  10.5e10,
  10.6E-10,
  .5,

  // boolean values
  true,
  false,
  TRUE,
  FALSE,

  // string values
  "string",
  'string',

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
// when $pieces argument is supplied with different values
echo "\n--- Testing join() by supplying different values for 'pieces' argument ---\n";
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
    echo "-- Iteration $counter --\n";
    $pieces = $values [$index];

    try {
        var_dump( join($glue, $pieces) );
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }

    $counter ++;
}

// close the resources used
fclose($fp);

echo "Done\n";
?>
--EXPECT--
*** Testing join() : usage variations ***

--- Testing join() by supplying different values for 'pieces' argument ---
-- Iteration 1 --
join(): Argument #2 ($array) must be of type array, int given
-- Iteration 2 --
join(): Argument #2 ($array) must be of type array, int given
-- Iteration 3 --
join(): Argument #2 ($array) must be of type array, int given
-- Iteration 4 --
join(): Argument #2 ($array) must be of type array, int given
-- Iteration 5 --
join(): Argument #2 ($array) must be of type array, float given
-- Iteration 6 --
join(): Argument #2 ($array) must be of type array, float given
-- Iteration 7 --
join(): Argument #2 ($array) must be of type array, float given
-- Iteration 8 --
join(): Argument #2 ($array) must be of type array, float given
-- Iteration 9 --
join(): Argument #2 ($array) must be of type array, float given
-- Iteration 10 --
join(): Argument #2 ($array) must be of type array, true given
-- Iteration 11 --
join(): Argument #2 ($array) must be of type array, false given
-- Iteration 12 --
join(): Argument #2 ($array) must be of type array, true given
-- Iteration 13 --
join(): Argument #2 ($array) must be of type array, false given
-- Iteration 14 --
join(): Argument #2 ($array) must be of type array, string given
-- Iteration 15 --
join(): Argument #2 ($array) must be of type array, string given
-- Iteration 16 --
join(): Argument #2 ($array) must be of type array, test given
-- Iteration 17 --
join(): Argument #2 ($array) must be of type array, string given
-- Iteration 18 --
join(): Argument #2 ($array) must be of type array, string given
-- Iteration 19 --
join(): Argument #2 ($array) must be of type array, null given
-- Iteration 20 --
join(): Argument #2 ($array) must be of type array, null given
-- Iteration 21 --
join(): Argument #2 ($array) must be of type array, resource given
-- Iteration 22 --
join(): Argument #2 ($array) must be of type array, null given
-- Iteration 23 --
join(): Argument #2 ($array) must be of type array, null given
Done
