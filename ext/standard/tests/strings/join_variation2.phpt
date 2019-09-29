--TEST--
Test join() function : usage variations - unexpected values for 'pieces' argument(Bug#42789)
--FILE--
<?php
/* Prototype  : string join( string $glue, array $pieces )
 * Description: Join array elements with a string
 * Source code: ext/standard/string.c
 * Alias of function: implode()
*/

/*
 * test join() by passing different unexpected value for pieces argument
*/

echo "*** Testing join() : usage variations ***\n";
// initialize all required variables
$glue = '::';

// get an unset variable
$unset_var = array(1, 2);
unset($unset_var);

// get a resouce variable
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
Invalid arguments passed
-- Iteration 2 --
Invalid arguments passed
-- Iteration 3 --
Invalid arguments passed
-- Iteration 4 --
Invalid arguments passed
-- Iteration 5 --
Invalid arguments passed
-- Iteration 6 --
Invalid arguments passed
-- Iteration 7 --
Invalid arguments passed
-- Iteration 8 --
Invalid arguments passed
-- Iteration 9 --
Invalid arguments passed
-- Iteration 10 --
Invalid arguments passed
-- Iteration 11 --
Invalid arguments passed
-- Iteration 12 --
Invalid arguments passed
-- Iteration 13 --
Invalid arguments passed
-- Iteration 14 --
Invalid arguments passed
-- Iteration 15 --
Invalid arguments passed
-- Iteration 16 --
Invalid arguments passed
-- Iteration 17 --
Invalid arguments passed
-- Iteration 18 --
Invalid arguments passed
-- Iteration 19 --
Invalid arguments passed
-- Iteration 20 --
Invalid arguments passed
-- Iteration 21 --
Invalid arguments passed
-- Iteration 22 --
Invalid arguments passed
-- Iteration 23 --
Invalid arguments passed
Done
