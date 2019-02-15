--TEST--
Test strval() function : usage variations  - Pass different data types as strval
--FILE--
<?php
/* Prototype  : string strval  ( mixed $var  )
 * Description: Get the string value of a variable.
 * Source code: ext/standard/string.c
 */

echo "*** Testing strval() : usage variations ***\n";

error_reporting(E_ALL ^ E_NOTICE);

//get an unset variable
$unset_var = 10;
unset ($unset_var);

//getting the resource
$file_handle = fopen(__FILE__, "r");

class MyClass
{
  function __toString() {
    return "MyClass";
  }
}

//array of values to iterate over
$values = array(
		  //Decimal values
/*1*/	  0,
		  1,
		  12345,
		  -12345,

		  //Octal values
/*5*/	  02,
		  010,
		  030071,
		  -030071,

		  //Hexadecimal values
/*9*/	  0x0,
		  0x1,
		  0xABCD,
		  -0xABCD,

	      // float data
/*13*/    100.5,
	      -100.5,
	      100.1234567e10,
	      100.7654321E-10,
	      .5,

	      // array data
/*18*/    array(),
	      array('color' => 'red', 'item' => 'pen'),

	      // null data
/*20*/    NULL,
	      null,

	      // boolean data
/*22*/    true,
	      false,
	      TRUE,
	      FALSE,

	      // empty data
/*26*/    "",
	      '',

	      // object data
/*28*/    new MyClass(),

	      // resource
/*29*/    $file_handle,

	      // undefined data
/*30*/    @$undefined_var,

	      // unset data
/*31*/    @$unset_var,
);

// loop through each element of the array for strval
$iterator = 1;
foreach($values as $value) {
      echo "\n-- Iteration $iterator --\n";
      var_dump( strval($value) );
      $iterator++;
};
?>
===DONE===
--EXPECTF--
*** Testing strval() : usage variations ***

-- Iteration 1 --
string(1) "0"

-- Iteration 2 --
string(1) "1"

-- Iteration 3 --
string(5) "12345"

-- Iteration 4 --
string(6) "-12345"

-- Iteration 5 --
string(1) "2"

-- Iteration 6 --
string(1) "8"

-- Iteration 7 --
string(5) "12345"

-- Iteration 8 --
string(6) "-12345"

-- Iteration 9 --
string(1) "0"

-- Iteration 10 --
string(1) "1"

-- Iteration 11 --
string(5) "43981"

-- Iteration 12 --
string(6) "-43981"

-- Iteration 13 --
string(5) "100.5"

-- Iteration 14 --
string(6) "-100.5"

-- Iteration 15 --
string(13) "1001234567000"

-- Iteration 16 --
string(14) "1.007654321E-8"

-- Iteration 17 --
string(3) "0.5"

-- Iteration 18 --
string(5) "Array"

-- Iteration 19 --
string(5) "Array"

-- Iteration 20 --
string(0) ""

-- Iteration 21 --
string(0) ""

-- Iteration 22 --
string(1) "1"

-- Iteration 23 --
string(0) ""

-- Iteration 24 --
string(1) "1"

-- Iteration 25 --
string(0) ""

-- Iteration 26 --
string(0) ""

-- Iteration 27 --
string(0) ""

-- Iteration 28 --
string(7) "MyClass"

-- Iteration 29 --
string(%d) "Resource id #%d"

-- Iteration 30 --
string(0) ""

-- Iteration 31 --
string(0) ""
===DONE===
