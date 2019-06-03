--TEST--
Test array_intersect_assoc() function : usage variations - unexpected values for 'arr1' argument(Bug#43196)
--FILE--
<?php
/* Prototype  : array array_intersect_assoc(array $arr1, array $arr2 [, array $...])
 * Description: Returns the entries of arr1 that have values which are present in all the other arguments.
 * Keys are used to do more restrictive check
 * Source code: ext/standard/array.c
*/

/*
* Testing array_intersect_assoc() function by passing values to $arr1 argument other than arrays
* and see that function emits proper warning messages wherever expected.
* The $arr2 argument passed is a fixed array.
*/

echo "*** Testing array_intersect_assoc() : Passing non-array values to \$arr1 argument ***\n";

// array to be passsed to $arr2 as default argument
$arr2 = array(1, 2);

// additional array to be passed for intersection
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

// loop through each sub-array within $arrrays to check the behavior of array_intersect_assoc()
$iterator = 1;
foreach($arrays as $unexpected_value) {
    echo "\n-- Iteration $iterator --";

    // Calling array_intersect_assoc() with default arguments
    try {
        var_dump( array_intersect_assoc($unexpected_value, $arr2) );
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }

    // Calling array_intersect_assoc() with more arguments
    try {
        var_dump( array_intersect_assoc($unexpected_value, $arr2, $arr3) );
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
    $iterator++;
}

// close the file resource used
fclose($fp);

echo "Done";
?>
--EXPECT--
*** Testing array_intersect_assoc() : Passing non-array values to $arr1 argument ***

-- Iteration 1 --Expected parameter 1 to be an array, int given
Expected parameter 1 to be an array, int given

-- Iteration 2 --Expected parameter 1 to be an array, int given
Expected parameter 1 to be an array, int given

-- Iteration 3 --Expected parameter 1 to be an array, int given
Expected parameter 1 to be an array, int given

-- Iteration 4 --Expected parameter 1 to be an array, int given
Expected parameter 1 to be an array, int given

-- Iteration 5 --Expected parameter 1 to be an array, float given
Expected parameter 1 to be an array, float given

-- Iteration 6 --Expected parameter 1 to be an array, float given
Expected parameter 1 to be an array, float given

-- Iteration 7 --Expected parameter 1 to be an array, float given
Expected parameter 1 to be an array, float given

-- Iteration 8 --Expected parameter 1 to be an array, float given
Expected parameter 1 to be an array, float given

-- Iteration 9 --Expected parameter 1 to be an array, float given
Expected parameter 1 to be an array, float given

-- Iteration 10 --Expected parameter 1 to be an array, null given
Expected parameter 1 to be an array, null given

-- Iteration 11 --Expected parameter 1 to be an array, null given
Expected parameter 1 to be an array, null given

-- Iteration 12 --Expected parameter 1 to be an array, bool given
Expected parameter 1 to be an array, bool given

-- Iteration 13 --Expected parameter 1 to be an array, bool given
Expected parameter 1 to be an array, bool given

-- Iteration 14 --Expected parameter 1 to be an array, bool given
Expected parameter 1 to be an array, bool given

-- Iteration 15 --Expected parameter 1 to be an array, bool given
Expected parameter 1 to be an array, bool given

-- Iteration 16 --Expected parameter 1 to be an array, string given
Expected parameter 1 to be an array, string given

-- Iteration 17 --Expected parameter 1 to be an array, string given
Expected parameter 1 to be an array, string given

-- Iteration 18 --Expected parameter 1 to be an array, string given
Expected parameter 1 to be an array, string given

-- Iteration 19 --Expected parameter 1 to be an array, string given
Expected parameter 1 to be an array, string given

-- Iteration 20 --Expected parameter 1 to be an array, string given
Expected parameter 1 to be an array, string given

-- Iteration 21 --Expected parameter 1 to be an array, object given
Expected parameter 1 to be an array, object given

-- Iteration 22 --Expected parameter 1 to be an array, null given
Expected parameter 1 to be an array, null given

-- Iteration 23 --Expected parameter 1 to be an array, null given
Expected parameter 1 to be an array, null given

-- Iteration 24 --Expected parameter 1 to be an array, resource given
Expected parameter 1 to be an array, resource given
Done
