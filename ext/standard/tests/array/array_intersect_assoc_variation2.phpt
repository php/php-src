--TEST--
Test array_intersect_assoc() function : usage variations - unexpected values for 'array2' argument(Bug#43196)
--FILE--
<?php
/*
* Testing array_intersect_assoc() function by passing values to $array2 argument other than arrays
* and see that function emits proper warning messages wherever expected.
* The $array1 argument passed is a fixed array.
*/

echo "*** Testing array_intersect_assoc() : Passing non-array values to \$array2 argument ***\n";

// array to be passsed to $array1 as default argument
$array1 = array(1, 2);

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

// unexpected values to be passed to $array2 argument
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

// loop through each sub-array within $arrays to check the behavior of array_intersect_assoc()
$iterator = 1;
foreach($arrays as $unexpected_value) {
    echo "\n-- Iteration $iterator --";

    // Calling array_intersect_assoc() with default arguments
    try {
        var_dump( array_intersect_assoc($array1,$unexpected_value) );
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }

    // Calling array_intersect_assoc() with more arguments
    try {
        var_dump( array_intersect_assoc($array1, $unexpected_value, $arr3) );
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
*** Testing array_intersect_assoc() : Passing non-array values to $array2 argument ***

-- Iteration 1 --array_intersect_assoc(): Argument #2 must be of type array, int given
array_intersect_assoc(): Argument #2 must be of type array, int given

-- Iteration 2 --array_intersect_assoc(): Argument #2 must be of type array, int given
array_intersect_assoc(): Argument #2 must be of type array, int given

-- Iteration 3 --array_intersect_assoc(): Argument #2 must be of type array, int given
array_intersect_assoc(): Argument #2 must be of type array, int given

-- Iteration 4 --array_intersect_assoc(): Argument #2 must be of type array, int given
array_intersect_assoc(): Argument #2 must be of type array, int given

-- Iteration 5 --array_intersect_assoc(): Argument #2 must be of type array, float given
array_intersect_assoc(): Argument #2 must be of type array, float given

-- Iteration 6 --array_intersect_assoc(): Argument #2 must be of type array, float given
array_intersect_assoc(): Argument #2 must be of type array, float given

-- Iteration 7 --array_intersect_assoc(): Argument #2 must be of type array, float given
array_intersect_assoc(): Argument #2 must be of type array, float given

-- Iteration 8 --array_intersect_assoc(): Argument #2 must be of type array, float given
array_intersect_assoc(): Argument #2 must be of type array, float given

-- Iteration 9 --array_intersect_assoc(): Argument #2 must be of type array, float given
array_intersect_assoc(): Argument #2 must be of type array, float given

-- Iteration 10 --array_intersect_assoc(): Argument #2 must be of type array, null given
array_intersect_assoc(): Argument #2 must be of type array, null given

-- Iteration 11 --array_intersect_assoc(): Argument #2 must be of type array, null given
array_intersect_assoc(): Argument #2 must be of type array, null given

-- Iteration 12 --array_intersect_assoc(): Argument #2 must be of type array, bool given
array_intersect_assoc(): Argument #2 must be of type array, bool given

-- Iteration 13 --array_intersect_assoc(): Argument #2 must be of type array, bool given
array_intersect_assoc(): Argument #2 must be of type array, bool given

-- Iteration 14 --array_intersect_assoc(): Argument #2 must be of type array, bool given
array_intersect_assoc(): Argument #2 must be of type array, bool given

-- Iteration 15 --array_intersect_assoc(): Argument #2 must be of type array, bool given
array_intersect_assoc(): Argument #2 must be of type array, bool given

-- Iteration 16 --array_intersect_assoc(): Argument #2 must be of type array, string given
array_intersect_assoc(): Argument #2 must be of type array, string given

-- Iteration 17 --array_intersect_assoc(): Argument #2 must be of type array, string given
array_intersect_assoc(): Argument #2 must be of type array, string given

-- Iteration 18 --array_intersect_assoc(): Argument #2 must be of type array, string given
array_intersect_assoc(): Argument #2 must be of type array, string given

-- Iteration 19 --array_intersect_assoc(): Argument #2 must be of type array, string given
array_intersect_assoc(): Argument #2 must be of type array, string given

-- Iteration 20 --array_intersect_assoc(): Argument #2 must be of type array, string given
array_intersect_assoc(): Argument #2 must be of type array, string given

-- Iteration 21 --array_intersect_assoc(): Argument #2 must be of type array, classA given
array_intersect_assoc(): Argument #2 must be of type array, classA given

-- Iteration 22 --array_intersect_assoc(): Argument #2 must be of type array, null given
array_intersect_assoc(): Argument #2 must be of type array, null given

-- Iteration 23 --array_intersect_assoc(): Argument #2 must be of type array, null given
array_intersect_assoc(): Argument #2 must be of type array, null given

-- Iteration 24 --array_intersect_assoc(): Argument #2 must be of type array, resource given
array_intersect_assoc(): Argument #2 must be of type array, resource given
Done
