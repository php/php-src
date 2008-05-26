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
  var_dump( array_merge_recursive($arr1, $arr2) );
  $iterator++;
}

// close the file resource used
fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing array_merge_recursive() : Passing non array values to $arr2 argument ***

-- Iteration 1 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 2 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 3 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 4 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 5 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 6 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 7 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 8 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 9 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 10 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 11 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 12 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 13 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 14 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 15 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 16 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 17 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 18 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 19 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 20 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 21 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 22 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 23 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL

-- Iteration 24 --
Warning: array_merge_recursive(): Argument #2 is not an array in %s on line %d
NULL
Done
