--TEST--
Test array_intersect() function : usage variations - unexpected values for 'arr1' argument
--FILE--
<?php
/* Prototype  : array array_intersect(array $arr1, array $arr2 [, array $...])
 * Description: Returns the entries of arr1 that have values which are present in all the other arguments
 * Source code: ext/standard/array.c
*/

/*
* Testing array_intersect() function by passing values to $arr1 argument other than arrays
* and see that function emits proper warning messages wherever expected.
* The $arr2 argument is a fixed array.
*/

echo "*** Testing array_intersect() : Passing non-array values to \$arr1 argument ***\n";

// array to be passsed to $arr2 as default argument
$arr2 = array(1, 2);

// array to be passed to optional argument
$arr3 = array(1, 2, "one" => 1, "two" => 2);

// get an unset variable
$unset_var = 10;
unset ($unset_var);

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

// unexpected values to be passed to $arr1 argument
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
/*22*/ @$undefined_var,

       // unset data
/*23*/ @$unset_var,

       // resource variable
/*24*/ $fp
);

// loop through each sub-array within $arrrays to check the behavior of array_intersect()
$iterator = 1;
foreach($arrays as $unexpected_value) {
  echo "\n-- Iterator $iterator --";

  // Calling array_intersect() with default arguments
  var_dump( array_intersect($unexpected_value,$arr2) );

  // Calling array_intersect() with more arguments
  var_dump( array_intersect($unexpected_value, $arr2, $arr3) );
  $iterator++;
}

// close the file resource used
fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing array_intersect() : Passing non-array values to $arr1 argument ***

-- Iterator 1 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 2 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 3 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 4 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 5 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 6 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 7 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 8 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 9 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 10 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 11 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 12 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 13 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 14 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 15 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 16 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 17 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 18 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 19 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 20 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 21 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 22 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 23 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

-- Iterator 24 --
Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL

Warning: array_intersect(): Argument #1 is not an array in %s on line %d
NULL
Done
