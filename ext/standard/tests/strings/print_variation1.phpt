--TEST--
Test print() function : usage variations 
--FILE--
<?php

/* Prototype  : int print  ( string $arg  )
 * Description: Output a string
 * Source code: n/a, print is a language construct not an extension function
 * Test based on php.net manual example.
*/

echo "*** Testing print() function: with unexpected inputs for 'arg' argument ***\n";

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
$inputs =  array (

		  // integer values
/*1*/	  0,
		  1,
		  -2,
		  2147483647,
		  -2147483648,
		
		  // float values
/*6*/	  10.5,
		  -20.5,
		  10.1234567e10,
		
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

// loop through with each element of the $inputs array to test print() function
$count = 1;
foreach($inputs as $input) {
  echo "-- Iteration $count --\n";
  $res = print($input);
  echo "\n";
  var_dump($res); 
  $count ++;
}

fclose($file_handle);  //closing the file handle

?>
===DONE===
--EXPECTF--
*** Testing print() function: with unexpected inputs for 'arg' argument ***
-- Iteration 1 --
0
int(1)
-- Iteration 2 --
1
int(1)
-- Iteration 3 --
-2
int(1)
-- Iteration 4 --
2147483647
int(1)
-- Iteration 5 --
-2147483648
int(1)
-- Iteration 6 --
10.5
int(1)
-- Iteration 7 --
-20.5
int(1)
-- Iteration 8 --
101234567000
int(1)
-- Iteration 9 --

Notice: Array to string conversion in %sprint_variation1.php on line %d
Array
int(1)
-- Iteration 10 --

Notice: Array to string conversion in %sprint_variation1.php on line %d
Array
int(1)
-- Iteration 11 --

Notice: Array to string conversion in %sprint_variation1.php on line %d
Array
int(1)
-- Iteration 12 --
1
int(1)
-- Iteration 13 --

int(1)
-- Iteration 14 --
1
int(1)
-- Iteration 15 --

int(1)
-- Iteration 16 --

int(1)
-- Iteration 17 --

int(1)
-- Iteration 18 --
sample object
int(1)
-- Iteration 19 --
Resource id #%d
int(1)
-- Iteration 20 --

int(1)
-- Iteration 21 --

int(1)
===DONE===