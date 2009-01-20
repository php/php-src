--TEST--
Test vfprintf() function : usage variations - unexpected values for the format argument
--FILE--
<?php
/* Prototype  : int vfprintf  ( resource $handle  , string $format , array $args  )
 * Description: Write a formatted string to a stream
 * Source code: ext/standard/formatted_print.c
*/

/*
 * Test vfprintf() when different unexpected format strings are passed to
 * the '$format' argument of the function
*/

echo "*** Testing vfprintf() : with unexpected values for format argument ***\n";

// initialising the required variables
$args = array(1, 2);

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// declaring a class
class sample
{
  public function __toString() {
  return "object";
  }
}

// Defining resource
$file_handle = fopen(__FILE__, 'r');


//array of values to iterate over
$values = array(

		  // int data
/*1*/	  0,
		  1,
		  12345,
		  -2345,
		
		  // float data
/*5*/	  10.5,
		  -10.5,
		  10.1234567e10,
		  10.7654321E-10,
		  .5,
		
		  // array data
/*10*/	  array(),
		  array(0),
		  array(1),
		  array(1,2),
		  array('color' => 'red', 'item' => 'pen'),
		
		  // null data
/*15*/	  NULL,
		  null,
		
		  // boolean data
/*17*/	  true,
		  false,
		  TRUE,
		  FALSE,
		
		  // empty data
/*21*/	  "",
		  '',
		
		  // object data
/*23*/	  new sample(),
		
		  // undefined data
/*24*/	  @$undefined_var,
		
		  // unset data
/*25*/	  @$unset_var,
		 
		  // resource data
/*26*/	  $file_handle
);

/* creating dumping file */
$data_file = dirname(__FILE__) . '/dump.txt';
if (!($fp = fopen($data_file, 'wt')))
   return;

fprintf($fp, "\n*** Testing vprintf() with with unexpected values for format argument ***\n");

$counter = 1;
foreach( $values as $value ) {
  fprintf( $fp, "\n-- Iteration %d --\n",$counter);
  vfprintf($fp, $value, $args);
  $counter++;
}

fclose($fp);
print_r(file_get_contents($data_file));
echo "\n";

unlink($data_file);

?>
===DONE===
--EXPECTF--
*** Testing vfprintf() : with unexpected values for format argument ***

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

Notice: Array to string conversion in %s on line %d

*** Testing vprintf() with with unexpected values for format argument ***

-- Iteration 1 --
0
-- Iteration 2 --
1
-- Iteration 3 --
12345
-- Iteration 4 --
-2345
-- Iteration 5 --
10.5
-- Iteration 6 --
-10.5
-- Iteration 7 --
101234567000
-- Iteration 8 --
1.07654321E-9
-- Iteration 9 --
0.5
-- Iteration 10 --
Array
-- Iteration 11 --
Array
-- Iteration 12 --
Array
-- Iteration 13 --
Array
-- Iteration 14 --
Array
-- Iteration 15 --

-- Iteration 16 --

-- Iteration 17 --
1
-- Iteration 18 --

-- Iteration 19 --
1
-- Iteration 20 --

-- Iteration 21 --

-- Iteration 22 --

-- Iteration 23 --
object
-- Iteration 24 --

-- Iteration 25 --

-- Iteration 26 --
Resource id #%d
===DONE===
