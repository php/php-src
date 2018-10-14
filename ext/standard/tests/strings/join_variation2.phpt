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

  // null vlaues
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

  var_dump( join($glue, $pieces) );

  $counter ++;
}

// close the resources used
fclose($fp);

echo "Done\n";
?>
--EXPECTF--
*** Testing join() : usage variations ***

--- Testing join() by supplying different values for 'pieces' argument ---
-- Iteration 1 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 2 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 3 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 4 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 5 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 6 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 7 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 8 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 9 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 10 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 11 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 12 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 13 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 14 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 15 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 16 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 17 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 18 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 19 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 20 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 21 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 22 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
-- Iteration 23 --

Warning: join(): Invalid arguments passed in %s on line %d
NULL
Done
