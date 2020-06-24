--TEST--
Test array_merge() function : usage variations - Pass different data types to $arr1 arg
--FILE--
<?php
/*
 * Pass different data types as $arr1 argument to test behaviour
 */

echo "*** Testing array_merge() : usage variations ***\n";

// Initialise function arguments not being substituted
$arr2 = array (1, 2);

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

// unexpected values to be passed to $arr1 argument
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
  var_dump( array_merge($input, $arr2) );
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing array_merge() : usage variations ***

-- Iteration 1 --

Fatal error: Uncaught TypeError: array_merge(): Argument #1 must be of type array, int given in %s:%d
Stack trace:
#0 %s(%d): array_merge(0, Array)
#1 {main}
  thrown in %s on line %d
