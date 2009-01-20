--TEST--
Test strip_tags() function : usage variations - unexpected values for 'str' argument
--INI--
set short_open_tag = on
--FILE--
<?php
/* Prototype  : string strip_tags(string $str [, string $allowable_tags])
 * Description: Strips HTML and PHP tags from a string 
 * Source code: ext/standard/string.c
*/

/*
 * testing functionality of strip_tags() by giving unexpected input values for $str argument
*/

echo "*** Testing strip_tags() : usage variations ***\n";

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//get a resource variable
$fp = fopen(__FILE__, "r");

//get a class
class classA{
  public function __toString(){
    return "Class A object";
  }
}

//array of values to iterate over
$values = array(

	      // int data
/*1*/     0,
	      1,
	      12345,
	      -2345,
	
	      // float data
/*5*/      10.5,
	      -10.5,
	      10.1234567e10,
	      10.7654321E-10,
	      .5,
	
	      // array data
/*10*/    array(),
	      array(0),
	      array(1),
	      array(1, 2),
	      array('color' => 'red', 'item' => 'pen'),
	
	      // null data
/*15*/    NULL,
	      null,
	
	      // boolean data
/*17*/    true,
	      false,
	      TRUE,
	      FALSE,
	
	      // empty data
/*21*/    "",
	      '',
	
	      // object data
/*23*/    new classA(),
	
	      // undefined data
/*24*/    @$undefined_var,
	
	      // unset data
/*25*/    @$unset_var,
	
	      // resource variable
/*26*/    $fp

);

// loop through each element of the array for allowable_tags
$iterator = 1;
foreach($values as $value) {
      echo "-- Iteration $iterator --\n";
      var_dump( strip_tags($value) );
      $iterator++;
};

?>
===DONE===
--EXPECTF--
*** Testing strip_tags() : usage variations ***
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
string(12) "101234567000"
-- Iteration 8 --
string(13) "1.07654321E-9"
-- Iteration 9 --
string(3) "0.5"
-- Iteration 10 --

Warning: strip_tags() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: strip_tags() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: strip_tags() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 13 --

Warning: strip_tags() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 14 --

Warning: strip_tags() expects parameter 1 to be string, array given in %s on line %d
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
string(0) ""
-- Iteration 22 --
string(0) ""
-- Iteration 23 --
string(14) "Class A object"
-- Iteration 24 --
string(0) ""
-- Iteration 25 --
string(0) ""
-- Iteration 26 --

Warning: strip_tags() expects parameter 1 to be string, resource given in %s on line %d
NULL
===DONE===