--TEST--
Test array_merge() function : usage variations - Pass different data types as $arr2 arg
--FILE--
<?php
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
    } catch (Throwable $e) {
        echo $e::class, ': ', $e->getMessage(), "\n";
    }
    $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECT--
*** Testing array_merge() : usage variations ***

-- Iteration 1 --
TypeError: array_merge(): Argument #2 must be of type array, int given

-- Iteration 2 --
TypeError: array_merge(): Argument #2 must be of type array, int given

-- Iteration 3 --
TypeError: array_merge(): Argument #2 must be of type array, int given

-- Iteration 4 --
TypeError: array_merge(): Argument #2 must be of type array, int given

-- Iteration 5 --
TypeError: array_merge(): Argument #2 must be of type array, float given

-- Iteration 6 --
TypeError: array_merge(): Argument #2 must be of type array, float given

-- Iteration 7 --
TypeError: array_merge(): Argument #2 must be of type array, float given

-- Iteration 8 --
TypeError: array_merge(): Argument #2 must be of type array, float given

-- Iteration 9 --
TypeError: array_merge(): Argument #2 must be of type array, float given

-- Iteration 10 --
TypeError: array_merge(): Argument #2 must be of type array, null given

-- Iteration 11 --
TypeError: array_merge(): Argument #2 must be of type array, null given

-- Iteration 12 --
TypeError: array_merge(): Argument #2 must be of type array, true given

-- Iteration 13 --
TypeError: array_merge(): Argument #2 must be of type array, false given

-- Iteration 14 --
TypeError: array_merge(): Argument #2 must be of type array, true given

-- Iteration 15 --
TypeError: array_merge(): Argument #2 must be of type array, false given

-- Iteration 16 --
TypeError: array_merge(): Argument #2 must be of type array, string given

-- Iteration 17 --
TypeError: array_merge(): Argument #2 must be of type array, string given

-- Iteration 18 --
array(2) {
  [0]=>
  int(1)
  [1]=>
  int(2)
}

-- Iteration 19 --
TypeError: array_merge(): Argument #2 must be of type array, string given

-- Iteration 20 --
TypeError: array_merge(): Argument #2 must be of type array, string given

-- Iteration 21 --
TypeError: array_merge(): Argument #2 must be of type array, string given

-- Iteration 22 --
TypeError: array_merge(): Argument #2 must be of type array, classA given

-- Iteration 23 --
TypeError: array_merge(): Argument #2 must be of type array, null given

-- Iteration 24 --
TypeError: array_merge(): Argument #2 must be of type array, null given

-- Iteration 25 --
TypeError: array_merge(): Argument #2 must be of type array, resource given
Done
