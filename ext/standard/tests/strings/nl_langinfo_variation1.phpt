--TEST--
Test nl_langinfo() function :  unexpected inputs for '$tem' argument
--SKIPIF--
<?php
if( substr(PHP_OS, 0, 3) == 'WIN'){  
  die('skip Not for Windows');
}
?>
--FILE--
<?php

/* Prototype  : string nl_langinfo  ( int $item  )
 * Description: Query language and locale information
 * Source code: ext/standard/string.c
*/

echo "*** Testing nl_langinfo() : with unexpected inputs for 'item' argument ***\n";

$original = setlocale(LC_ALL, 'C');

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

// array with different values for $input
$items =  array (

		  // integer values
/*1*/	  0,
		  10,
	      2147483647,
		  -2147483648,
		
		  // float values
/*5*/	  10.5,
		  20.3,
		  -20.5,
		  10.1234567e5,
		
		  // array values
/*9*/	  array(),
		  array(0),
		  array(1, 2),
		
		  // boolean values
/*12*/	  true,
		  false,
		  TRUE,
		  FALSE,
		
		  // null vlaues
/*16*/	  NULL,
		  null,
		
		  // objects
/*18*/	  new sample(),
		
		  // resource
/*19*/	  $file_handle,
		
		  // undefined variable
/*20*/	  @$undefined_var,
		
		  // unset variable
/*21*/	  @$unset_var
);

//defining '$input' argument
$input = "Test string";

// loop through with each element of the $items array to test nl_langinfo() function
$count = 1;
foreach($items as $item) {
  echo "-- Iteration $count --\n";
  var_dump( nl_langinfo($item) );
  $count ++;
}

fclose($file_handle);  //closing the file handle
setlocale(LC_ALL, $original); 

?>
===DONE===
--EXPECTF--
*** Testing nl_langinfo() : with unexpected inputs for 'item' argument ***
-- Iteration 1 --

Warning: nl_langinfo(): Item '0' is not valid in %s on line %d
bool(false)
-- Iteration 2 --

Warning: nl_langinfo(): Item '10' is not valid in %s on line %d
bool(false)
-- Iteration 3 --

Warning: nl_langinfo(): Item '2147483647' is not valid in %s on line %d
bool(false)
-- Iteration 4 --

Warning: nl_langinfo(): Item '-2147483648' is not valid in %s on line %d
bool(false)
-- Iteration 5 --

Warning: nl_langinfo(): Item '10' is not valid in %s on line %d
bool(false)
-- Iteration 6 --

Warning: nl_langinfo(): Item '20' is not valid in %s on line %d
bool(false)
-- Iteration 7 --

Warning: nl_langinfo(): Item '-20' is not valid in %s on line %d
bool(false)
-- Iteration 8 --

Warning: nl_langinfo(): Item '1012345' is not valid in %s on line %d
bool(false)
-- Iteration 9 --

Warning: nl_langinfo() expects parameter 1 to be long, array given in %s on line %d
NULL
-- Iteration 10 --

Warning: nl_langinfo() expects parameter 1 to be long, array given in %s on line %d
NULL
-- Iteration 11 --

Warning: nl_langinfo() expects parameter 1 to be long, array given in %s on line %d
NULL
-- Iteration 12 --

Warning: nl_langinfo(): Item '1' is not valid in %s on line %d
bool(false)
-- Iteration 13 --

Warning: nl_langinfo(): Item '0' is not valid in %s on line %d
bool(false)
-- Iteration 14 --

Warning: nl_langinfo(): Item '1' is not valid in %s on line %d
bool(false)
-- Iteration 15 --

Warning: nl_langinfo(): Item '0' is not valid in %s on line %d
bool(false)
-- Iteration 16 --

Warning: nl_langinfo(): Item '0' is not valid in %s on line %d
bool(false)
-- Iteration 17 --

Warning: nl_langinfo(): Item '0' is not valid in %s on line %d
bool(false)
-- Iteration 18 --

Warning: nl_langinfo() expects parameter 1 to be long, object given in %s on line %d
NULL
-- Iteration 19 --

Warning: nl_langinfo() expects parameter 1 to be long, resource given in %s on line %d
NULL
-- Iteration 20 --

Warning: nl_langinfo(): Item '0' is not valid in %s on line %d
bool(false)
-- Iteration 21 --

Warning: nl_langinfo(): Item '0' is not valid in %s on line %d
bool(false)
===DONE===
