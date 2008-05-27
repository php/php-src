--TEST--
Test chunk_split() function : usage variations - with unexpected values for 'str' argument
--FILE--
<?php
/* Prototype  : string chunk_split(string $str [, int $chunklen [, string $ending]])
 * Description: Returns split line 
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

echo "*** Testing chunk_split() : with unexpected values for 'str' argument ***\n";

// Initialising variables
$chunklen = 2;
$ending = ' ';

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//class for object variable
class MyClass
{
  public function __toString()
  {
    return "object";
  }
}

//resource  variable
$fp = fopen(__FILE__, 'r');

//different values for 'str'
$values = array(

  // int data
  0,
  1,
  12345,
  -2345,

  // float data
  10.5,
  -10.5,
  10.1234567e10,
  10.7654321E-10,
  .5,

  // array data
  array(),
  array(0),
  array(1),
  array(1, 2),
  array('color' => 'red', 'item' => 'pen'),

  // null data
  NULL,
  null,

  // boolean data
  true,
  false,
  TRUE,
  FALSE,

  // empty data
  "",
  '',

  // string data
  "string",
  'string',

  // object data
  new MyClass(),

  // undefined data
  @$undefined_var,

  // unset data
  @$unset_var,

  // resource data
  $fp	
);

// loop through each element of the array for 'str'
for($count = 0; $count < count($values); $count++) {
  echo "-- Iteration ".($count+1)." --\n";
  var_dump( chunk_split($values[$count], $chunklen, $ending) );
};

echo "Done";

// close the resource
fclose($fp);

?>
--EXPECTF--
*** Testing chunk_split() : with unexpected values for 'str' argument ***
-- Iteration 1 --
unicode(2) "0 "
-- Iteration 2 --
unicode(2) "1 "
-- Iteration 3 --
unicode(8) "12 34 5 "
-- Iteration 4 --
unicode(8) "-2 34 5 "
-- Iteration 5 --
unicode(6) "10 .5 "
-- Iteration 6 --
unicode(8) "-1 0. 5 "
-- Iteration 7 --
unicode(18) "10 12 34 56 70 00 "
-- Iteration 8 --
unicode(20) "1. 07 65 43 21 E- 9 "
-- Iteration 9 --
unicode(5) "0. 5 "
-- Iteration 10 --

Warning: chunk_split() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 11 --

Warning: chunk_split() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 12 --

Warning: chunk_split() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 13 --

Warning: chunk_split() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 14 --

Warning: chunk_split() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 15 --
unicode(1) " "
-- Iteration 16 --
unicode(1) " "
-- Iteration 17 --
unicode(2) "1 "
-- Iteration 18 --
unicode(1) " "
-- Iteration 19 --
unicode(2) "1 "
-- Iteration 20 --
unicode(1) " "
-- Iteration 21 --
unicode(1) " "
-- Iteration 22 --
unicode(1) " "
-- Iteration 23 --
unicode(9) "st ri ng "
-- Iteration 24 --
unicode(9) "st ri ng "
-- Iteration 25 --
unicode(9) "ob je ct "
-- Iteration 26 --
unicode(1) " "
-- Iteration 27 --
unicode(1) " "
-- Iteration 28 --

Warning: chunk_split() expects parameter 1 to be string (Unicode or binary), resource given in %s on line %d
NULL
Done
