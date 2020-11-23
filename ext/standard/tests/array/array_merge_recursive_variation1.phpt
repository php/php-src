--TEST--
Test array_merge_recursive() function : usage variations - unexpected values for $arr1 argument
--FILE--
<?php
/*
 * Passing non array values to 'arr1' argument of array_merge_recursive() and see
 * that the function outputs proper warning messages wherever expected.
*/

echo "*** Testing array_merge_recursive() : Passing non array values to \$arr1 argument ***\n";

//get an unset variable
$unset_var = 10;
unset($unset_var);

class A
{
//  public $var = 10;
  public function __toString() {
    return "object";
  }
}

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $arr1 argument
$arrays = array (

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

       // undefined data
/*21*/ @$undefined_var,

       // unset data
/*22*/ @$unset_var,

       // resource variable
/*23*/ $fp,

       // object data
/*24*/ new A()
);

// initialise the second argument
$arr2 = array(1, array("hello", 'world'));

// loop through each element of $arrays and check the behavior of array_merge_recursive()
$iterator = 1;
foreach($arrays as $arr1) {
    echo "\n-- Iteration $iterator --";

    // with default argument
    echo "\n-- With default argument --";
    try {
        var_dump( array_merge_recursive($arr1) );
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }

    // with more arguments
    echo "-- With more arguments --";
    try {
        var_dump( array_merge_recursive($arr1, $arr2) );
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
*** Testing array_merge_recursive() : Passing non array values to $arr1 argument ***

-- Iteration 1 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, int given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, int given

-- Iteration 2 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, int given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, int given

-- Iteration 3 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, int given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, int given

-- Iteration 4 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, int given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, int given

-- Iteration 5 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, float given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, float given

-- Iteration 6 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, float given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, float given

-- Iteration 7 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, float given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, float given

-- Iteration 8 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, float given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, float given

-- Iteration 9 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, float given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, float given

-- Iteration 10 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, null given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, null given

-- Iteration 11 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, null given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, null given

-- Iteration 12 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, bool given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, bool given

-- Iteration 13 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, bool given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, bool given

-- Iteration 14 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, bool given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, bool given

-- Iteration 15 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, bool given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, bool given

-- Iteration 16 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, string given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, string given

-- Iteration 17 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, string given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, string given

-- Iteration 18 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, string given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, string given

-- Iteration 19 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, string given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, string given

-- Iteration 20 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, string given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, string given

-- Iteration 21 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, null given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, null given

-- Iteration 22 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, null given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, null given

-- Iteration 23 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, resource given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, resource given

-- Iteration 24 --
-- With default argument --array_merge_recursive(): Argument #1 must be of type array, A given
-- With more arguments --array_merge_recursive(): Argument #1 must be of type array, A given
Done
