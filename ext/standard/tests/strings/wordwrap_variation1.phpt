--TEST--
Test wordwrap() function : usage variations  - unexpected values for str argument
--FILE--
<?php
/* Prototype  : string wordwrap ( string $str [, int $width [, string $break [, bool $cut]]] )
 * Description: Wraps buffer to selected number of characters using string break char
 * Source code: ext/standard/string.c
*/

/*
 * testing wordwrap() by providing different values for str argument
*/

echo "*** Testing wordwrap() : usage variations ***\n";
// initialize all required variables
$width = 3;
$break = '<br />\n';
$cut = true;

// resource variable
$fp = fopen(__FILE__, "r");

// get an unset variable
$unset_var = 'string_val';
unset($unset_var);

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
  new stdclass(),

  // Null
  NULL,
  null,

  // empty string
  "",
  '',

  // resource variable
  $fp,

  // undefined variable
  @$undefined_var,

  // unset variable
  @$unset_var
);

// loop though each element of the array and check the working of wordwrap()
// when $str argument is supplied with different values
echo "\n--- Testing wordwrap() by supplying different values for 'str' argument ---\n";
$counter = 1;
for($index = 0; $index < count($values); $index ++) {
  echo "-- Iteration $counter --\n";
  $str = $values [$index];

  var_dump( wordwrap($str) );
  var_dump( wordwrap($str, $width) );
  var_dump( wordwrap($str, $width, $break) );

  // $cut as false
  $cut = false;
  var_dump( wordwrap($str, $width, $break, $cut) );

  // $cut as true
  $cut = true;
  var_dump( wordwrap($str, $width, $break, $cut) );

  $counter ++;
}

// close the resource
fclose($fp);

echo "Done\n";
?>
--EXPECTF--
*** Testing wordwrap() : usage variations ***

--- Testing wordwrap() by supplying different values for 'str' argument ---
-- Iteration 1 --
string(1) "0"
string(1) "0"
string(1) "0"
string(1) "0"
string(1) "0"
-- Iteration 2 --
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
-- Iteration 3 --
string(5) "12345"
string(5) "12345"
string(5) "12345"
string(5) "12345"
string(13) "123<br />\n45"
-- Iteration 4 --
string(5) "-2345"
string(5) "-2345"
string(5) "-2345"
string(5) "-2345"
string(13) "-23<br />\n45"
-- Iteration 5 --
string(4) "10.5"
string(4) "10.5"
string(4) "10.5"
string(4) "10.5"
string(12) "10.<br />\n5"
-- Iteration 6 --
string(5) "-10.5"
string(5) "-10.5"
string(5) "-10.5"
string(5) "-10.5"
string(13) "-10<br />\n.5"
-- Iteration 7 --
string(12) "101234567000"
string(12) "101234567000"
string(12) "101234567000"
string(12) "101234567000"
string(36) "101<br />\n234<br />\n567<br />\n000"
-- Iteration 8 --
string(13) "1.07654321E-9"
string(13) "1.07654321E-9"
string(13) "1.07654321E-9"
string(13) "1.07654321E-9"
string(45) "1.0<br />\n765<br />\n432<br />\n1E-<br />\n9"
-- Iteration 9 --
string(3) "0.5"
string(3) "0.5"
string(3) "0.5"
string(3) "0.5"
string(3) "0.5"
-- Iteration 10 --

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 13 --

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 14 --

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 15 --
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
-- Iteration 16 --
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
-- Iteration 17 --
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
string(1) "1"
-- Iteration 18 --
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
-- Iteration 19 --

Warning: wordwrap() expects parameter 1 to be string, object given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, object given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, object given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, object given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, object given in %s on line %d
NULL
-- Iteration 20 --
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
-- Iteration 21 --
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
-- Iteration 22 --
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
-- Iteration 23 --
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
-- Iteration 24 --

Warning: wordwrap() expects parameter 1 to be string, resource given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, resource given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, resource given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, resource given in %s on line %d
NULL

Warning: wordwrap() expects parameter 1 to be string, resource given in %s on line %d
NULL
-- Iteration 25 --
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
-- Iteration 26 --
string(0) ""
string(0) ""
string(0) ""
string(0) ""
string(0) ""
Done
