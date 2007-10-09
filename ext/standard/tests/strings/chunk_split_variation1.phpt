--TEST--
Test chunk_split() function : usage variations - with unexpected values for 'str' argument
--FILE--
<?php
/* Prototype  : string chunk_split(string $str [, int $chunklen [, string $ending]])
 * Description: Returns split line %d%d
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
  10.5e10,
  10.6E-10,
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
string(2) "0 "
-- Iteration 2 --
string(2) "1 "
-- Iteration 3 --
string(8) "12 34 5 "
-- Iteration 4 --
string(8) "-2 34 5 "
-- Iteration 5 --
string(6) "10 .5 "
-- Iteration 6 --
string(8) "-1 0. 5 "
-- Iteration 7 --
string(18) "10 50 00 00 00 00 "
-- Iteration 8 --
string(11) "1. 06 E- 9 "
-- Iteration 9 --
string(5) "0. 5 "
-- Iteration 10 --

Notice: Array to string conversion in %s on line %d
string(8) "Ar ra y "
-- Iteration 11 --

Notice: Array to string conversion in %s on line %d
string(8) "Ar ra y "
-- Iteration 12 --

Notice: Array to string conversion in %s on line %d
string(8) "Ar ra y "
-- Iteration 13 --

Notice: Array to string conversion in %s on line %d
string(8) "Ar ra y "
-- Iteration 14 --

Notice: Array to string conversion in %s on line %d
string(8) "Ar ra y "
-- Iteration 15 --
string(1) " "
-- Iteration 16 --
string(1) " "
-- Iteration 17 --
string(2) "1 "
-- Iteration 18 --
string(1) " "
-- Iteration 19 --
string(2) "1 "
-- Iteration 20 --
string(1) " "
-- Iteration 21 --
string(1) " "
-- Iteration 22 --
string(1) " "
-- Iteration 23 --
string(9) "st ri ng "
-- Iteration 24 --
string(9) "st ri ng "
-- Iteration 25 --
string(9) "ob je ct "
-- Iteration 26 --
string(1) " "
-- Iteration 27 --
string(1) " "
-- Iteration 28 --
string(%d) "Re so ur ce  i d  #%d "
Done
