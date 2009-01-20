--TEST--
Test strip_tags() function : usage variations - unexpected values for both 'str' and 'allowable_tags'
--INI--
set short_open_tag = on
--FILE--
<?php
/* Prototype  : string strip_tags(string $str [, string $allowable_tags])
 * Description: Strips HTML and PHP tags from a string 
 * Source code: ext/standard/string.c
*/

/*
 * testing functionality of strip_tags() by giving unexpected values for $str and $allowable_tags arguments
*/

echo "*** Testing strip_tags() : usage variations ***\n";

// get a resource
$fp=fopen(__FILE__, "r");

// get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA{
  public function __toString(){
    return "Class A object";
  }
}

//array of values to iterate over
$values = array(

		      // int data
/*1*/	      0,
		      1,
		      12345,
		      -2345,
		
		      // float data
/*5*/	      10.5,
		      -10.5,
		      10.1234567e10,
		      10.7654321E-10,
		      .5,
		
		      // array data
/*10*/	      array(),
		      array(0),
		      array(1),
		      array(1, 2),
		      array('color' => 'red', 'item' => 'pen'),
		
		      // null data
/*15*/	      NULL,
		      null,
		
		      // boolean data
/*17*/	      true,
		      false,
		      TRUE,
		      FALSE,
		
		      // empty data
/*21*/	      "",
		      '',
		
		      // object data
/*23*/	      new classA(),
		
		      // undefined data
/*24*/	      @$undefined_var,
		
		      // unset data
/*25*/	      @$unset_var,
		
		      // resource variable
/*26*/	      $fp
);

// loop through each element of the array for allowable_tags
$iterator = 1;
foreach($values as $value) {
      echo "-- Iteration $iterator --\n";
      var_dump( strip_tags($value, $value) );
      $iterator++;
};

?>
===DONE===
--EXPECTF--
*** Testing strip_tags() : usage variations ***
-- Iteration 1 --
unicode(1) "0"
-- Iteration 2 --
unicode(1) "1"
-- Iteration 3 --
unicode(5) "12345"
-- Iteration 4 --
unicode(5) "-2345"
-- Iteration 5 --
unicode(4) "10.5"
-- Iteration 6 --
unicode(5) "-10.5"
-- Iteration 7 --
unicode(12) "101234567000"
-- Iteration 8 --
unicode(13) "1.07654321E-9"
-- Iteration 9 --
unicode(3) "0.5"
-- Iteration 10 --

Warning: strip_tags() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 11 --

Warning: strip_tags() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 12 --

Warning: strip_tags() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 13 --

Warning: strip_tags() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 14 --

Warning: strip_tags() expects parameter 1 to be string (Unicode or binary), array given in %s on line %d
NULL
-- Iteration 15 --
unicode(0) ""
-- Iteration 16 --
unicode(0) ""
-- Iteration 17 --
unicode(1) "1"
-- Iteration 18 --
unicode(0) ""
-- Iteration 19 --
unicode(1) "1"
-- Iteration 20 --
unicode(0) ""
-- Iteration 21 --
unicode(0) ""
-- Iteration 22 --
unicode(0) ""
-- Iteration 23 --
unicode(14) "Class A object"
-- Iteration 24 --
unicode(0) ""
-- Iteration 25 --
unicode(0) ""
-- Iteration 26 --

Warning: strip_tags() expects parameter 1 to be string (Unicode or binary), resource given in %s on line %d
NULL
===DONE===