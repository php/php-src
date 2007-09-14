--TEST--
Test wordwrap() function : usage variations  - unexptected values for break argument
--INI--
--FILE--
<?php
/* Prototype  : string wordwrap ( string $str [, int $width [, string $break [, bool $cut]]] )
 * Description: Wraps buffer to selected number of characters using string break char
 * Source code: ext/standard/string.c
*/

/*
 * test wordwrap by passing different values for break argument 
*/
echo "*** Testing wordwrap() : usage variations ***\n";
// initialize all required variables
$str = 'testing wordwrap function';
$width = 10;
$cut = true;

// resource var
$fp = fopen(__FILE__, "r");

// get an unset variable
$unset_var = 'string_val';
unset($unset_var);


// array with different values for break arg
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
  new stdclass(),

  // empty string
  "",
  '',

  //Null
  NULL,
  null,

  // resource var
  $fp,

  // undefined variable
  @$undefined_var,

  // unset variable
  @$unset_var
);

// loop though each element of the array and check the working of wordwrap()
// when $break arugment is supplied with different values
echo "\n--- Testing wordwrap() by supplying different values for 'break' argument ---\n";
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  $break = $values [$index];

  var_dump( wordwrap($str, $width, $break) );

  // $cut as false
  $cut = false;
  var_dump( wordwrap($str, $width, $break, $cut) );

  // $cut as true 
  $cut = true;
  var_dump( wordwrap($str, $width, $break, $cut) );

  $counter ++;
}

// close the resource used
fclose($fp);

echo "Done\n";
?>
--EXPECTF--
*** Testing wordwrap() : usage variations ***

--- Testing wordwrap() by supplying different values for 'break' argument ---
-- Iteration 1 --
string(25) "testing0wordwrap0function"
string(25) "testing0wordwrap0function"
string(25) "testing0wordwrap0function"
-- Iteration 2 --
string(25) "testing1wordwrap1function"
string(25) "testing1wordwrap1function"
string(25) "testing1wordwrap1function"
-- Iteration 3 --
string(33) "testing12345wordwrap12345function"
string(33) "testing12345wordwrap12345function"
string(33) "testing12345wordwrap12345function"
-- Iteration 4 --
string(33) "testing-2345wordwrap-2345function"
string(33) "testing-2345wordwrap-2345function"
string(33) "testing-2345wordwrap-2345function"
-- Iteration 5 --
string(31) "testing10.5wordwrap10.5function"
string(31) "testing10.5wordwrap10.5function"
string(31) "testing10.5wordwrap10.5function"
-- Iteration 6 --
string(33) "testing-10.5wordwrap-10.5function"
string(33) "testing-10.5wordwrap-10.5function"
string(33) "testing-10.5wordwrap-10.5function"
-- Iteration 7 --
string(47) "testing105000000000wordwrap105000000000function"
string(47) "testing105000000000wordwrap105000000000function"
string(47) "testing105000000000wordwrap105000000000function"
-- Iteration 8 --
string(37) "testing1.06E-9wordwrap1.06E-9function"
string(37) "testing1.06E-9wordwrap1.06E-9function"
string(37) "testing1.06E-9wordwrap1.06E-9function"
-- Iteration 9 --
string(29) "testing0.5wordwrap0.5function"
string(29) "testing0.5wordwrap0.5function"
string(29) "testing0.5wordwrap0.5function"
-- Iteration 10 --

Warning: wordwrap() expects parameter 3 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 3 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 3 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: wordwrap() expects parameter 3 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 3 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 3 to be string, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: wordwrap() expects parameter 3 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 3 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 3 to be string, array given in %s on line %d
NULL
-- Iteration 13 --

Warning: wordwrap() expects parameter 3 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 3 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 3 to be string, array given in %s on line %d
NULL
-- Iteration 14 --

Warning: wordwrap() expects parameter 3 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 3 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 3 to be string, array given in %s on line %d
NULL
-- Iteration 15 --
string(25) "testing1wordwrap1function"
string(25) "testing1wordwrap1function"
string(25) "testing1wordwrap1function"
-- Iteration 16 --

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)
-- Iteration 17 --
string(25) "testing1wordwrap1function"
string(25) "testing1wordwrap1function"
string(25) "testing1wordwrap1function"
-- Iteration 18 --

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)
-- Iteration 19 --

Warning: wordwrap() expects parameter 3 to be string, object given in %s on line %d
NULL

Warning: wordwrap() expects parameter 3 to be string, object given in %s on line %d
NULL

Warning: wordwrap() expects parameter 3 to be string, object given in %s on line %d
NULL
-- Iteration 20 --

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)
-- Iteration 21 --

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)
-- Iteration 22 --

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)
-- Iteration 23 --

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)
-- Iteration 24 --

Warning: wordwrap() expects parameter 3 to be string, resource given in %s on line %d
NULL

Warning: wordwrap() expects parameter 3 to be string, resource given in %s on line %d
NULL

Warning: wordwrap() expects parameter 3 to be string, resource given in %s on line %d
NULL
-- Iteration 25 --

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)
-- Iteration 26 --

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)

Warning: wordwrap(): Break string cannot be empty in %s on line %d
bool(false)
Done
