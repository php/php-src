--TEST--
Test chunk_split() function : usage variations - unexpected values for 'ending' argument
--FILE--
<?php
/* Prototype  : string chunk_split(string $str [, int $chunklen [, string $ending]])
 * Description: Returns split line
 * Source code: ext/standard/string.c
 * Alias to functions: none
*/

echo "*** Testing chunk_split() : unexpected values for 'ending' ***\n";

// Initializing variables
$str = 'This is simple string.';
$chunklen = 4.9;

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//resource variable
$fp = fopen(__FILE__,'r');

//Class to get object variable
class MyClass
{
  public function __toString()
  {
    return "object";
  }
}

//different values for 'ending'
$values = array(

  // int data
  0,
  1,
  12345,
  -2345,

  // float data
  10.5,
  -10.5,
  10.123456e10,
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

  // object data
  new MyClass(),

  // undefined data
  @$undefined_var,

  // unset data
  @$unset_var,

  // resource data
  $fp
);

// loop through each element of values for 'ending'
for($count = 0; $count < count($values); $count++) {
  echo "-- Iteration ".($count+1)." --\n";
  var_dump( chunk_split($str, $chunklen, $values[$count]) );
}

echo "Done";

//closing resource
fclose($fp);
?>
--EXPECTF--
*** Testing chunk_split() : unexpected values for 'ending' ***
-- Iteration 1 --
string(28) "This0 is 0simp0le s0trin0g.0"
-- Iteration 2 --
string(28) "This1 is 1simp1le s1trin1g.1"
-- Iteration 3 --
string(52) "This12345 is 12345simp12345le s12345trin12345g.12345"
-- Iteration 4 --
string(52) "This-2345 is -2345simp-2345le s-2345trin-2345g.-2345"
-- Iteration 5 --
string(46) "This10.5 is 10.5simp10.5le s10.5trin10.5g.10.5"
-- Iteration 6 --
string(52) "This-10.5 is -10.5simp-10.5le s-10.5trin-10.5g.-10.5"
-- Iteration 7 --
string(94) "This101234560000 is 101234560000simp101234560000le s101234560000trin101234560000g.101234560000"
-- Iteration 8 --
string(100) "This1.07654321E-9 is 1.07654321E-9simp1.07654321E-9le s1.07654321E-9trin1.07654321E-9g.1.07654321E-9"
-- Iteration 9 --
string(40) "This0.5 is 0.5simp0.5le s0.5trin0.5g.0.5"
-- Iteration 10 --

Warning: chunk_split() expects parameter 3 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: chunk_split() expects parameter 3 to be string, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: chunk_split() expects parameter 3 to be string, array given in %s on line %d
NULL
-- Iteration 13 --

Warning: chunk_split() expects parameter 3 to be string, array given in %s on line %d
NULL
-- Iteration 14 --

Warning: chunk_split() expects parameter 3 to be string, array given in %s on line %d
NULL
-- Iteration 15 --
string(22) "This is simple string."
-- Iteration 16 --
string(22) "This is simple string."
-- Iteration 17 --
string(28) "This1 is 1simp1le s1trin1g.1"
-- Iteration 18 --
string(22) "This is simple string."
-- Iteration 19 --
string(28) "This1 is 1simp1le s1trin1g.1"
-- Iteration 20 --
string(22) "This is simple string."
-- Iteration 21 --
string(22) "This is simple string."
-- Iteration 22 --
string(22) "This is simple string."
-- Iteration 23 --
string(58) "Thisobject is objectsimpobjectle sobjecttrinobjectg.object"
-- Iteration 24 --
string(22) "This is simple string."
-- Iteration 25 --
string(22) "This is simple string."
-- Iteration 26 --

Warning: chunk_split() expects parameter 3 to be string, resource given in %s on line %d
NULL
Done
