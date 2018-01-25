--TEST--
Test nl2br() function : usage variations - unexpected values for 'str' argument
--FILE--
<?php
/* Prototype  : string nl2br(string $str)
 * Description: Inserts HTML line breaks before all newlines in a string.
 * Source code: ext/standard/string.c
*/

/*
* Test nl2br() function by passing different types of values other than
*   expected type for 'str' argument
*/

echo "*** Testing nl2br() : usage variations ***\n";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//getting resource
$file_handle = fopen(__FILE__, "r");

//defining class
class Sample {
  public function __toString() {
    return "My String";
  }
}

//array of values to iterate over
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

  //resource
  $file_handle,

  // object data
  new Sample(),

  // undefined data
  @$undefined_var,

  // unset data
  @$unset_var,
);

// loop through $values array to test nl2br() function with each element
$count = 1;
foreach($values as $value) {
  echo "-- Iteration $count --\n";
  var_dump( nl2br($value) );
  $count ++ ;
};

//closing the file handle
fclose( $file_handle );

echo "Done";
?>
--EXPECTF--
*** Testing nl2br() : usage variations ***
-- Iteration 1 --
string(1) "0"
-- Iteration 2 --
string(1) "1"
-- Iteration 3 --
string(5) "12345"
-- Iteration 4 --
string(5) "-2345"
-- Iteration 5 --
string(4) "10.5"
-- Iteration 6 --
string(5) "-10.5"
-- Iteration 7 --
string(12) "105000000000"
-- Iteration 8 --
string(7) "1.06E-9"
-- Iteration 9 --
string(3) "0.5"
-- Iteration 10 --

Warning: nl2br() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: nl2br() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: nl2br() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 13 --

Warning: nl2br() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 14 --

Warning: nl2br() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 15 --
string(0) ""
-- Iteration 16 --
string(0) ""
-- Iteration 17 --
string(1) "1"
-- Iteration 18 --
string(0) ""
-- Iteration 19 --
string(1) "1"
-- Iteration 20 --
string(0) ""
-- Iteration 21 --

Warning: nl2br() expects parameter 1 to be string, resource given in %s on line %d
NULL
-- Iteration 22 --
string(9) "My String"
-- Iteration 23 --
string(0) ""
-- Iteration 24 --
string(0) ""
Done
