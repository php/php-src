--TEST--
Test rsort() function : usage variations - Pass different data types as $sort_flags arg
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64-bit only");
--FILE--
<?php
/* Prototype  : bool rsort(array &$array_arg [, int $sort_flags])
 * Description: Sort an array in reverse order 
 * Source code: ext/standard/array.c
 */

/*
 * Pass different data types as $sort_flags argument to rsort() to test behaviour
 * Where possible, 'SORT_NUMERIC' has been entered as a string value
 */

echo "*** Testing rsort() : variation ***\n";

// Initialise function arguments not being substituted
$array_arg = array (1, 5, 2, 3, 1);

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
  public function __toString() {
    return "SORT_NUMERIC";
  }
}

// heredoc string
$heredoc = <<<EOT
SORT_NUMERIC
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $sort_flags argument
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

       // string data
/*18*/ "SORT_NUMERIC",
       'SORT_NUMERIC',
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

// loop through each element of $inputs to check the behavior of rsort()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  
  //create temporary array in case rsort() works
  $temp = $array_arg;
  
  var_dump( rsort($temp, $input) );
  var_dump($temp);
  $iterator++;
  
  $temp = null;
};

fclose($fp);

echo "Done";
?>

--EXPECTF--
*** Testing rsort() : variation ***

-- Iteration 1 --
bool(true)
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(3)
  [2]=>
  int(2)
  [3]=>
  int(1)
  [4]=>
  int(1)
}

-- Iteration 2 --
bool(true)
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(3)
  [2]=>
  int(2)
  [3]=>
  int(1)
  [4]=>
  int(1)
}

-- Iteration 3 --
bool(true)
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(3)
  [2]=>
  int(2)
  [3]=>
  int(1)
  [4]=>
  int(1)
}

-- Iteration 4 --
bool(true)
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(3)
  [2]=>
  int(2)
  [3]=>
  int(1)
  [4]=>
  int(1)
}

-- Iteration 5 --
bool(true)
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(3)
  [2]=>
  int(2)
  [3]=>
  int(1)
  [4]=>
  int(1)
}

-- Iteration 6 --
bool(true)
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(3)
  [2]=>
  int(2)
  [3]=>
  int(1)
  [4]=>
  int(1)
}

-- Iteration 7 --
bool(true)
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(3)
  [2]=>
  int(2)
  [3]=>
  int(1)
  [4]=>
  int(1)
}

-- Iteration 8 --
bool(true)
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(3)
  [2]=>
  int(2)
  [3]=>
  int(1)
  [4]=>
  int(1)
}

-- Iteration 9 --
bool(true)
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(3)
  [2]=>
  int(2)
  [3]=>
  int(1)
  [4]=>
  int(1)
}

-- Iteration 10 --
bool(true)
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(3)
  [2]=>
  int(2)
  [3]=>
  int(1)
  [4]=>
  int(1)
}

-- Iteration 11 --
bool(true)
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(3)
  [2]=>
  int(2)
  [3]=>
  int(1)
  [4]=>
  int(1)
}

-- Iteration 12 --
bool(true)
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(3)
  [2]=>
  int(2)
  [3]=>
  int(1)
  [4]=>
  int(1)
}

-- Iteration 13 --
bool(true)
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(3)
  [2]=>
  int(2)
  [3]=>
  int(1)
  [4]=>
  int(1)
}

-- Iteration 14 --
bool(true)
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(3)
  [2]=>
  int(2)
  [3]=>
  int(1)
  [4]=>
  int(1)
}

-- Iteration 15 --
bool(true)
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(3)
  [2]=>
  int(2)
  [3]=>
  int(1)
  [4]=>
  int(1)
}

-- Iteration 16 --

Warning: rsort() expects parameter 2 to be integer, string given in %s on line %d
bool(false)
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(5)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(1)
}

-- Iteration 17 --

Warning: rsort() expects parameter 2 to be integer, string given in %s on line %d
bool(false)
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(5)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(1)
}

-- Iteration 18 --

Warning: rsort() expects parameter 2 to be integer, string given in %s on line %d
bool(false)
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(5)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(1)
}

-- Iteration 19 --

Warning: rsort() expects parameter 2 to be integer, string given in %s on line %d
bool(false)
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(5)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(1)
}

-- Iteration 20 --

Warning: rsort() expects parameter 2 to be integer, string given in %s on line %d
bool(false)
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(5)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(1)
}

-- Iteration 21 --

Warning: rsort() expects parameter 2 to be integer, object given in %s on line %d
bool(false)
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(5)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(1)
}

-- Iteration 22 --
bool(true)
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(3)
  [2]=>
  int(2)
  [3]=>
  int(1)
  [4]=>
  int(1)
}

-- Iteration 23 --
bool(true)
array(5) {
  [0]=>
  int(5)
  [1]=>
  int(3)
  [2]=>
  int(2)
  [3]=>
  int(1)
  [4]=>
  int(1)
}

-- Iteration 24 --

Warning: rsort() expects parameter 2 to be integer, resource given in %s on line %d
bool(false)
array(5) {
  [0]=>
  int(1)
  [1]=>
  int(5)
  [2]=>
  int(2)
  [3]=>
  int(3)
  [4]=>
  int(1)
}
Done
