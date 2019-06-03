--TEST--
Test array_merge() function : usage variations - Pass different data types as $arr2 arg
--FILE--
<?php
/* Prototype  : array array_merge(array $arr1, array $arr2 [, array $...])
 * Description: Merges elements from passed arrays into one array
 * Source code: ext/standard/array.c
 */

/*
 * Pass different data types as $arr2 argument to array_merge() to test behaviour
 */

echo "*** Testing array_merge() : usage variations ***\n";

// Initialise function arguments not being substituted
$arr1 = array (1, 2);

//get an unset variable
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

// unexpected values to be passed to $arr2 argument
$inputs = array(

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
       array(),

       // string data
/*19*/ "string",
       'string',
       $heredoc,

       // object data
/*22*/ new classA(),

       // undefined data
/*23*/ @$undefined_var,

       // unset data
/*24*/ @$unset_var,

       // resource variable
/*25*/ $fp
);

// loop through each element of $inputs to check the behavior of array_merge()
$iterator = 1;
foreach($inputs as $input) {
    echo "\n-- Iteration $iterator --\n";
    try {
        var_dump( array_merge($arr1, $input) );
    } catch (TypeError $e) {
        echo $e->getMessage(), "\n";
    }
    $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECT--
*** Testing array_merge() : usage variations ***

-- Iteration 1 --
Expected parameter 2 to be an array, int given

-- Iteration 2 --
Expected parameter 2 to be an array, int given

-- Iteration 3 --
Expected parameter 2 to be an array, int given

-- Iteration 4 --
Expected parameter 2 to be an array, int given

-- Iteration 5 --
Expected parameter 2 to be an array, float given

-- Iteration 6 --
Expected parameter 2 to be an array, float given

-- Iteration 7 --
Expected parameter 2 to be an array, float given

-- Iteration 8 --
Expected parameter 2 to be an array, float given

-- Iteration 9 --
Expected parameter 2 to be an array, float given

-- Iteration 10 --
Expected parameter 2 to be an array, null given

-- Iteration 11 --
Expected parameter 2 to be an array, null given

-- Iteration 12 --
Expected parameter 2 to be an array, bool given

-- Iteration 13 --
Expected parameter 2 to be an array, bool given

-- Iteration 14 --
Expected parameter 2 to be an array, bool given

-- Iteration 15 --
Expected parameter 2 to be an array, bool given

-- Iteration 16 --
Expected parameter 2 to be an array, string given

-- Iteration 17 --
Expected parameter 2 to be an array, string given

-- Iteration 18 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}

-- Iteration 19 --
Expected parameter 2 to be an array, string given

-- Iteration 20 --
Expected parameter 2 to be an array, string given

-- Iteration 21 --
Expected parameter 2 to be an array, string given

-- Iteration 22 --
Expected parameter 2 to be an array, object given

-- Iteration 23 --
Expected parameter 2 to be an array, null given

-- Iteration 24 --
Expected parameter 2 to be an array, null given

-- Iteration 25 --
Expected parameter 2 to be an array, resource given
Done
