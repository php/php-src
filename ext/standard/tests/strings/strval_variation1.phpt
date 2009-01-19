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
unicode(1) "0"

-- Iteration 2 --
unicode(1) "1"

-- Iteration 3 --
unicode(5) "12345"

-- Iteration 4 --
unicode(6) "-12345"

-- Iteration 5 --
unicode(1) "2"

-- Iteration 6 --
unicode(1) "8"

-- Iteration 7 --
unicode(5) "12345"

-- Iteration 8 --
unicode(6) "-12345"

-- Iteration 9 --
unicode(1) "0"

-- Iteration 10 --
unicode(1) "1"

-- Iteration 11 --
unicode(5) "43981"

-- Iteration 12 --
unicode(6) "-43981"

-- Iteration 13 --
unicode(5) "100.5"

-- Iteration 14 --
unicode(6) "-100.5"

-- Iteration 15 --
unicode(13) "1001234567000"

-- Iteration 16 --
unicode(14) "1.007654321E-8"

-- Iteration 17 --
unicode(3) "0.5"

-- Iteration 18 --
unicode(5) "Array"

-- Iteration 19 --
unicode(5) "Array"

-- Iteration 20 --
unicode(0) ""

-- Iteration 21 --
unicode(0) ""

-- Iteration 22 --
unicode(1) "1"

-- Iteration 23 --
unicode(0) ""

-- Iteration 24 --
unicode(1) "1"

-- Iteration 25 --
unicode(0) ""

-- Iteration 26 --
unicode(0) ""

-- Iteration 27 --
unicode(0) ""

-- Iteration 28 --
unicode(7) "MyClass"

-- Iteration 29 --
unicode(%d) "Resource id #%d"

-- Iteration 30 --
unicode(0) ""

-- Iteration 31 --
unicode(0) ""
===DONE===