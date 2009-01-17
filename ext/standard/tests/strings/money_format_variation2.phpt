--TEST--
Test money_format() function :  usage variations - test values for $number argument
--SKIPIF--
<?php
	if (!function_exists('money_format') || !function_exists('setlocale')) {
		die("SKIP money_format - not supported\n");
	}
	
	if (setlocale(LC_MONETARY, 'de_DE') == false) {
		die("SKIP de_DE locale not available\n");
	}	
?>
--FILE--
<?php
/* Prototype  : string money_format  ( string $format  , float $number  )
 * Description: Formats a number as a currency string
 * Source code: ext/standard/string.c
*/

$original = setlocale(LC_MONETARY, 'en_US');


echo "*** Testing money_format() function: with unexpected inputs for 'number' argument ***\n";

//get an unset variable
$unset_var = '  string_val  ';
unset($unset_var);

//defining a couple of sample classes
class class_no_tostring  {
}
 
class class_with_tostring  {
  public function __toString() {
    return "  sample object  ";
  } 
}


//getting the resource
$file_handle = fopen(__FILE__, "r");

// array with different values for $number
$numbers =  array (

		  // integer values
/*1*/	  0,
		  1,
		  255,
		  256,
	      2147483647,
		  -2147483648,
		
		  // float values
/*7*/	  10.5,
		  -20.5,
		  10.1234567e10,
		
		  // array values
/*10*/	  array(),
		  array(0),
		  array(1, 2),
		
		  // boolean values
/*13*/	  true,
		  false,
		  TRUE,
		  FALSE,
		
		  // null values
/*17*/	  NULL,
		  null,
		  
		  // string values
/*19*/	  "abcd",
		  'abcd',
		  "0x12f",
		  "%=*!14#8.2nabcd",
		
		  // objects
/*23*/	  new class_no_tostring(),
		  new class_with_tostring(),
		
		  // resource
/*25*/	  $file_handle,
		
		  // undefined variable
/*26*/	  @$undefined_var,
		
		  // unset variable
/*27*/	  @$unset_var
);

// loop through with each element of the $numbers array to test money_format() function
$count = 1;
$format = '%14#8.2i';

foreach($numbers as $number) {
  echo "-- Iteration $count --\n";
  var_dump( money_format($format, $number) );
  $count ++;
}

// close the file handle
fclose($file_handle);  

// restore original locale
setlocale(LC_MONETARY, $original);

?>
===DONE===
--EXPECTF--
*** Testing money_format() function: with unexpected inputs for 'number' argument ***
-- Iteration 1 --
string(18) " USD          0.00"
-- Iteration 2 --
string(18) " USD          1.00"
-- Iteration 3 --
string(18) " USD        255.00"
-- Iteration 4 --
string(18) " USD        256.00"
-- Iteration 5 --
string(21) " USD 2,147,483,647.00"
-- Iteration 6 --
string(21) "-USD 2,147,483,648.00"
-- Iteration 7 --
string(18) " USD         10.50"
-- Iteration 8 --
string(18) "-USD         20.50"
-- Iteration 9 --
string(23) " USD 101,234,567,000.00"
-- Iteration 10 --

Warning: money_format() expects parameter 2 to be double, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: money_format() expects parameter 2 to be double, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: money_format() expects parameter 2 to be double, array given in %s on line %d
NULL
-- Iteration 13 --
string(18) " USD          1.00"
-- Iteration 14 --
string(18) " USD          0.00"
-- Iteration 15 --
string(18) " USD          1.00"
-- Iteration 16 --
string(18) " USD          0.00"
-- Iteration 17 --
string(18) " USD          0.00"
-- Iteration 18 --
string(18) " USD          0.00"
-- Iteration 19 --

Warning: money_format() expects parameter 2 to be double, string given in %s on line %d
NULL
-- Iteration 20 --

Warning: money_format() expects parameter 2 to be double, string given in %s on line %d
NULL
-- Iteration 21 --
string(18) " USD        303.00"
-- Iteration 22 --

Warning: money_format() expects parameter 2 to be double, string given in %s on line %d
NULL
-- Iteration 23 --

Warning: money_format() expects parameter 2 to be double, object given in %s on line %d
NULL
-- Iteration 24 --

Warning: money_format() expects parameter 2 to be double, object given in %s on line %d
NULL
-- Iteration 25 --

Warning: money_format() expects parameter 2 to be double, resource given in %s on line %d
NULL
-- Iteration 26 --
string(18) " USD          0.00"
-- Iteration 27 --
string(18) " USD          0.00"
===DONE===
