--TEST--
Test strtr() function : usage variations - unexpected inputs for 'str' argument
--FILE--
<?php
/* Prototype  : string strtr(string $str, string $from[, string $to]);
                string strtr(string $str, array $replace_pairs);
 * Description: Translates characters in str using given translation tables
 * Source code: ext/standard/string.c
*/

/* Test strtr() function: with unexpected inputs for 'str'
 *  and expected type for 'from' & 'to' arguments
*/

echo "*** Testing strtr() function: with unexpected inputs for 'str' ***\n";

//get an unset variable
$unset_var = 'string_val';
unset($unset_var);

//defining a class
class sample  {
  public function __toString() {
    return "sample object";
  }
}

//getting the resource
$file_handle = fopen(__FILE__, "r");

// array with different values
$strings =  array (

		  // integer values
/*1*/	  0,
		  1,
		  -2,

		  // float values
/*4*/	  10.5,
		  -20.5,
		  10.1234567e10,

		  // array values
/*7*/	  array(),
		  array(0),
		  array(1, 2),

		  // boolean values
/*10*/	  true,
		  false,
		  TRUE,
		  FALSE,

		  // null values
/*14*/	  NULL,
		  null,

		  // objects
/*16*/	  new sample(),

		  // resource
/*17*/	  $file_handle,

		  // undefined variable
/*18*/	  @$undefined_var,

		  // unset variable
/*19*/	  @$unset_var
);

//defining 'from' argument
$from = "012atm";

//defining 'to' argument
$to = "atm012";

// loop through with each element of the $strings array to test strtr() function
$count = 1;
for($index = 0; $index < count($strings); $index++) {
  echo "-- Iteration $count --\n";
  $str = $strings[$index];
  var_dump( strtr($str, $from, $to) );
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing strtr() function: with unexpected inputs for 'str' ***
-- Iteration 1 --
string(1) "a"
-- Iteration 2 --
string(1) "t"
-- Iteration 3 --
string(2) "-m"
-- Iteration 4 --
string(4) "ta.5"
-- Iteration 5 --
string(5) "-ma.5"
-- Iteration 6 --
string(12) "tatm34567aaa"
-- Iteration 7 --

Warning: strtr() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 8 --

Warning: strtr() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 9 --

Warning: strtr() expects parameter 1 to be string, array given in %s on line %d
NULL
-- Iteration 10 --
string(1) "t"
-- Iteration 11 --
string(0) ""
-- Iteration 12 --
string(1) "t"
-- Iteration 13 --
string(0) ""
-- Iteration 14 --
string(0) ""
-- Iteration 15 --
string(0) ""
-- Iteration 16 --
string(13) "s02ple objec1"
-- Iteration 17 --

Warning: strtr() expects parameter 1 to be string, resource given in %s on line %d
NULL
-- Iteration 18 --
string(0) ""
-- Iteration 19 --
string(0) ""
===DONE===
