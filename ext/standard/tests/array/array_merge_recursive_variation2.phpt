--TEST--
Test array_merge_recursive() function : usage variations - unexpected values for $arr2 argument
--FILE--
<?php
/* Prototype  : array array_merge_recursive(array $arr1[, array $...])
 * Description: Recursively merges elements from passed arrays into one array
 * Source code: ext/standard/array.c
*/

/*
 * Passing non array values to 'arr2' argument of array_merge_recursive() and see
 * that the function outputs proper warning messages wherever expected.
*/

echo "*** Testing array_merge_recursive() : Passing non array values to \$arr2 argument ***\n";

// initialise the first argument
$arr1 = array(1, array("hello", 'world'));

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

// unexpected values to be passed to $arr2 argument
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

// loop through each element of $arrays and check the behavior of array_merge_recursive()
$iterator = 1;
foreach($arrays as $arr2) {
    echo "\n-- Iteration $iterator --";
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
*** Testing array_merge_recursive() : Passing non array values to $arr2 argument ***

-- Iteration 1 --Expected parameter 2 to be an array, int given

-- Iteration 2 --Expected parameter 2 to be an array, int given

-- Iteration 3 --Expected parameter 2 to be an array, int given

-- Iteration 4 --Expected parameter 2 to be an array, int given

-- Iteration 5 --Expected parameter 2 to be an array, float given

-- Iteration 6 --Expected parameter 2 to be an array, float given

-- Iteration 7 --Expected parameter 2 to be an array, float given

-- Iteration 8 --Expected parameter 2 to be an array, float given

-- Iteration 9 --Expected parameter 2 to be an array, float given

-- Iteration 10 --Expected parameter 2 to be an array, null given

-- Iteration 11 --Expected parameter 2 to be an array, null given

-- Iteration 12 --Expected parameter 2 to be an array, bool given

-- Iteration 13 --Expected parameter 2 to be an array, bool given

-- Iteration 14 --Expected parameter 2 to be an array, bool given

-- Iteration 15 --Expected parameter 2 to be an array, bool given

-- Iteration 16 --Expected parameter 2 to be an array, string given

-- Iteration 17 --Expected parameter 2 to be an array, string given

-- Iteration 18 --Expected parameter 2 to be an array, string given

-- Iteration 19 --Expected parameter 2 to be an array, string given

-- Iteration 20 --Expected parameter 2 to be an array, string given

-- Iteration 21 --Expected parameter 2 to be an array, null given

-- Iteration 22 --Expected parameter 2 to be an array, null given

-- Iteration 23 --Expected parameter 2 to be an array, resource given

-- Iteration 24 --Expected parameter 2 to be an array, object given
Done
