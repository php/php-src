--TEST--
Test array_unshift() function : usage variations - unexpected values for 'array' argument
--FILE--
<?php
/* Prototype  : int array_unshift(array $array, mixed $var [, mixed ...])
 * Description: Pushes elements onto the beginning of the array
 * Source code: ext/standard/array.c
*/

/*
 * Testing the behavior of array_unshift() by giving values
 * other than array values for $array argument
*/

echo "*** Testing array_unshift() : unexpected values for \$array argument ***\n";

// Initialise $var argument
$var = 12;

// get a class
class classA
{
  public function __toString() {
    return "Class A object";
  }
}

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// get an unset variable
$unset_var = 10;
unset ($unset_var);

// array of unexpected values to be passed to $array argument
$arrays = array(

       // int data
/*1*/  0,
       1,
       12345,
       -2345,

       // float data
/*5*/  10.5,
       -10.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // null data
/*10*/ NULL,
       null,

       // boolean data
/*12*/ true,
       false,
       TRUE,
       FALSE,

       // empty data
/*16*/ "",
       '',

       // string data
/*18*/ "string",
       'string',
       $heredoc,

       // object data
/*21*/ new classA(),

       // undefined data
       @$undefined_var,

       // unset data
       @$unset_var,

       // resource variable
/*24*/ $fp
);

// loop through each element of $arrays to test the functionality of array_unshift()
$iterator = 1;
foreach($arrays as $array) {
  echo "\n-- Iteration $iterator --";

  /* with default arguments */
  // returns element count in the resulting array after arguments are pushed to
  // beginning of the given array
  $temp_array = $array;
  var_dump( array_unshift($temp_array, $var) );

  // dump the resulting array
  var_dump($temp_array);

  /* with optional arguments */
  // returns element count in the resulting array after arguments are pushed to
  // beginning of the given array
  $temp_array = $array;
  var_dump( array_unshift($temp_array, $var, "hello", 'world') );

  // dump the resulting array
  var_dump($temp_array);
  $iterator++;
};

// close the file resource used
fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing array_unshift() : unexpected values for $array argument ***

-- Iteration 1 --
Warning: array_unshift() expects parameter 1 to be array, int given in %s on line %d
NULL
int(0)

Warning: array_unshift() expects parameter 1 to be array, int given in %s on line %d
NULL
int(0)

-- Iteration 2 --
Warning: array_unshift() expects parameter 1 to be array, int given in %s on line %d
NULL
int(1)

Warning: array_unshift() expects parameter 1 to be array, int given in %s on line %d
NULL
int(1)

-- Iteration 3 --
Warning: array_unshift() expects parameter 1 to be array, int given in %s on line %d
NULL
int(12345)

Warning: array_unshift() expects parameter 1 to be array, int given in %s on line %d
NULL
int(12345)

-- Iteration 4 --
Warning: array_unshift() expects parameter 1 to be array, int given in %s on line %d
NULL
int(-2345)

Warning: array_unshift() expects parameter 1 to be array, int given in %s on line %d
NULL
int(-2345)

-- Iteration 5 --
Warning: array_unshift() expects parameter 1 to be array, float given in %s on line %d
NULL
float(10.5)

Warning: array_unshift() expects parameter 1 to be array, float given in %s on line %d
NULL
float(10.5)

-- Iteration 6 --
Warning: array_unshift() expects parameter 1 to be array, float given in %s on line %d
NULL
float(-10.5)

Warning: array_unshift() expects parameter 1 to be array, float given in %s on line %d
NULL
float(-10.5)

-- Iteration 7 --
Warning: array_unshift() expects parameter 1 to be array, float given in %s on line %d
NULL
float(123456789000)

Warning: array_unshift() expects parameter 1 to be array, float given in %s on line %d
NULL
float(123456789000)

-- Iteration 8 --
Warning: array_unshift() expects parameter 1 to be array, float given in %s on line %d
NULL
float(1.23456789E-9)

Warning: array_unshift() expects parameter 1 to be array, float given in %s on line %d
NULL
float(1.23456789E-9)

-- Iteration 9 --
Warning: array_unshift() expects parameter 1 to be array, float given in %s on line %d
NULL
float(0.5)

Warning: array_unshift() expects parameter 1 to be array, float given in %s on line %d
NULL
float(0.5)

-- Iteration 10 --
Warning: array_unshift() expects parameter 1 to be array, null given in %s on line %d
NULL
NULL

Warning: array_unshift() expects parameter 1 to be array, null given in %s on line %d
NULL
NULL

-- Iteration 11 --
Warning: array_unshift() expects parameter 1 to be array, null given in %s on line %d
NULL
NULL

Warning: array_unshift() expects parameter 1 to be array, null given in %s on line %d
NULL
NULL

-- Iteration 12 --
Warning: array_unshift() expects parameter 1 to be array, bool given in %s on line %d
NULL
bool(true)

Warning: array_unshift() expects parameter 1 to be array, bool given in %s on line %d
NULL
bool(true)

-- Iteration 13 --
Warning: array_unshift() expects parameter 1 to be array, bool given in %s on line %d
NULL
bool(false)

Warning: array_unshift() expects parameter 1 to be array, bool given in %s on line %d
NULL
bool(false)

-- Iteration 14 --
Warning: array_unshift() expects parameter 1 to be array, bool given in %s on line %d
NULL
bool(true)

Warning: array_unshift() expects parameter 1 to be array, bool given in %s on line %d
NULL
bool(true)

-- Iteration 15 --
Warning: array_unshift() expects parameter 1 to be array, bool given in %s on line %d
NULL
bool(false)

Warning: array_unshift() expects parameter 1 to be array, bool given in %s on line %d
NULL
bool(false)

-- Iteration 16 --
Warning: array_unshift() expects parameter 1 to be array, string given in %s on line %d
NULL
string(0) ""

Warning: array_unshift() expects parameter 1 to be array, string given in %s on line %d
NULL
string(0) ""

-- Iteration 17 --
Warning: array_unshift() expects parameter 1 to be array, string given in %s on line %d
NULL
string(0) ""

Warning: array_unshift() expects parameter 1 to be array, string given in %s on line %d
NULL
string(0) ""

-- Iteration 18 --
Warning: array_unshift() expects parameter 1 to be array, string given in %s on line %d
NULL
string(6) "string"

Warning: array_unshift() expects parameter 1 to be array, string given in %s on line %d
NULL
string(6) "string"

-- Iteration 19 --
Warning: array_unshift() expects parameter 1 to be array, string given in %s on line %d
NULL
string(6) "string"

Warning: array_unshift() expects parameter 1 to be array, string given in %s on line %d
NULL
string(6) "string"

-- Iteration 20 --
Warning: array_unshift() expects parameter 1 to be array, string given in %s on line %d
NULL
string(11) "hello world"

Warning: array_unshift() expects parameter 1 to be array, string given in %s on line %d
NULL
string(11) "hello world"

-- Iteration 21 --
Warning: array_unshift() expects parameter 1 to be array, object given in %s on line %d
NULL
object(classA)#1 (0) {
}

Warning: array_unshift() expects parameter 1 to be array, object given in %s on line %d
NULL
object(classA)#1 (0) {
}

-- Iteration 22 --
Warning: array_unshift() expects parameter 1 to be array, null given in %s on line %d
NULL
NULL

Warning: array_unshift() expects parameter 1 to be array, null given in %s on line %d
NULL
NULL

-- Iteration 23 --
Warning: array_unshift() expects parameter 1 to be array, null given in %s on line %d
NULL
NULL

Warning: array_unshift() expects parameter 1 to be array, null given in %s on line %d
NULL
NULL

-- Iteration 24 --
Warning: array_unshift() expects parameter 1 to be array, resource given in %s on line %d
NULL
resource(%d) of type (stream)

Warning: array_unshift() expects parameter 1 to be array, resource given in %s on line %d
NULL
resource(%d) of type (stream)
Done
