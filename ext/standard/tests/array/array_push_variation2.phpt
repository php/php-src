--TEST--
Test array_push() function : usage variations - Pass different data types as $var arg
--FILE--
<?php
/* Prototype  : int array_push(array $stack, mixed $var [, mixed $...])
 * Description: Pushes elements onto the end of the array
 * Source code: ext/standard/array.c
 */

/*
 * Pass different data types as $var argument to array_push to test behaviour
 */

echo "*** Testing array_push() : usage variations ***\n";

// Initialise function arguments not being substituted
$stack = array (1, 2);

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

// unexpected values to be passed to $var argument
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

// loop through each element of $inputs to check the behavior of array_push()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  $temp_array = $stack;
  var_dump( array_push($temp_array, $input) );
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing array_push() : usage variations ***

-- Iteration 1 --
int(3)

-- Iteration 2 --
int(3)

-- Iteration 3 --
int(3)

-- Iteration 4 --
int(3)

-- Iteration 5 --
int(3)

-- Iteration 6 --
int(3)

-- Iteration 7 --
int(3)

-- Iteration 8 --
int(3)

-- Iteration 9 --
int(3)

-- Iteration 10 --
int(3)

-- Iteration 11 --
int(3)

-- Iteration 12 --
int(3)

-- Iteration 13 --
int(3)

-- Iteration 14 --
int(3)

-- Iteration 15 --
int(3)

-- Iteration 16 --
int(3)

-- Iteration 17 --
int(3)

-- Iteration 18 --
int(3)

-- Iteration 19 --
int(3)

-- Iteration 20 --
int(3)

-- Iteration 21 --
int(3)

-- Iteration 22 --
int(3)

-- Iteration 23 --
int(3)

-- Iteration 24 --
int(3)

-- Iteration 25 --
int(3)
Done
