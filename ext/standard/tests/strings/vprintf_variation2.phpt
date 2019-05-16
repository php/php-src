--TEST--
Test vprintf() function : usage variations - unexpected values for args argument
--FILE--
<?php
/* Prototype  : string vprintf(string format, array args)
 * Description: Output a formatted string
 * Source code: ext/standard/formatted_print.c
*/

/*
 * Test vprintf() when different unexpected values are passed to
 * the '$args' arguments of the function
*/

echo "*** Testing vprintf() : with unexpected values for args argument ***\n";

// initialising the required variables
$format = '%s';

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

		  // null data
/*10*/	  NULL,
		  null,

		  // boolean data
/*12*/	  true,
		  false,
		  TRUE,
		  FALSE,

		  // empty data
/*16*/	  "",
		  '',

		  // string data
/*18*/	  "string",
		  'string',

		  // object data
/*20*/	  new sample(),

		  // undefined data
/*21*/	  @$undefined_var,

		  // unset data
/*22*/	  @$unset_var,

		  // resource data
/*23*/		  $file_handle
);

// loop through each element of the array for args
$counter = 1;
foreach($values as $value) {
  echo "\n-- Iteration $counter --\n";
  $result = vprintf($format,$value);
  echo "\n";
  var_dump($result);
  $counter++;
};

// closing the resource
fclose($file_handle);

?>
===DONE===
--EXPECTF--
*** Testing vprintf() : with unexpected values for args argument ***

-- Iteration 1 --
0
int(1)

-- Iteration 2 --
1
int(1)

-- Iteration 3 --
12345
int(5)

-- Iteration 4 --
-2345
int(5)

-- Iteration 5 --
10.5
int(4)

-- Iteration 6 --
-10.5
int(5)

-- Iteration 7 --
101234567000
int(12)

-- Iteration 8 --
1.07654321E-9
int(13)

-- Iteration 9 --
0.5
int(3)

-- Iteration 10 --

Warning: vprintf(): Too few arguments in %s on line %d

bool(false)

-- Iteration 11 --

Warning: vprintf(): Too few arguments in %s on line %d

bool(false)

-- Iteration 12 --
1
int(1)

-- Iteration 13 --

int(0)

-- Iteration 14 --
1
int(1)

-- Iteration 15 --

int(0)

-- Iteration 16 --

int(0)

-- Iteration 17 --

int(0)

-- Iteration 18 --
string
int(6)

-- Iteration 19 --
string
int(6)

-- Iteration 20 --

Warning: vprintf(): Too few arguments in %s on line %d

bool(false)

-- Iteration 21 --

Warning: vprintf(): Too few arguments in %s on line %d

bool(false)

-- Iteration 22 --

Warning: vprintf(): Too few arguments in %s on line %d

bool(false)

-- Iteration 23 --
Resource id #%d
int(%d)
===DONE===
