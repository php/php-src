--TEST--
Test scandir() function : usage variations - diff data types as $sorting_order arg
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64-bit only");
--FILE--
<?php
/* Prototype  : array scandir(string $dir [, int $sorting_order [, resource $context]])
 * Description: List files & directories inside the specified path
 * Source code: ext/standard/dir.c
 */

/*
 * Pass different data types as $sorting_order argument to test how scandir() behaves
 */

echo "*** Testing scandir() : usage variations ***\n";

// Initialise function arguments not being substituted
$dir = dirname(__FILE__) . '/私はガラスを食べられますscandir_variation2';
mkdir($dir);

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

// unexpected values to be passed to $sorting_order argument
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

// loop through each element of $inputs to check the behavior of scandir()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( scandir($dir, $input) );
  $iterator++;
};

fclose($fp);
?>
===DONE===
--CLEAN--
<?php
$dir = dirname(__FILE__) . '/私はガラスを食べられますscandir_variation2';
rmdir($dir);
?>
--EXPECTF--
*** Testing scandir() : usage variations ***

-- Iteration 1 --
array(2) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
}

-- Iteration 2 --
array(2) {
  [0]=>
  string(2) ".."
  [1]=>
  string(1) "."
}

-- Iteration 3 --
array(2) {
  [0]=>
  string(2) ".."
  [1]=>
  string(1) "."
}

-- Iteration 4 --
array(2) {
  [0]=>
  string(2) ".."
  [1]=>
  string(1) "."
}

-- Iteration 5 --
array(2) {
  [0]=>
  string(2) ".."
  [1]=>
  string(1) "."
}

-- Iteration 6 --
array(2) {
  [0]=>
  string(2) ".."
  [1]=>
  string(1) "."
}

-- Iteration 7 --
array(2) {
  [0]=>
  string(2) ".."
  [1]=>
  string(1) "."
}

-- Iteration 8 --
array(2) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
}

-- Iteration 9 --
array(2) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
}

-- Iteration 10 --
array(2) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
}

-- Iteration 11 --
array(2) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
}

-- Iteration 12 --
array(2) {
  [0]=>
  string(2) ".."
  [1]=>
  string(1) "."
}

-- Iteration 13 --
array(2) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
}

-- Iteration 14 --
array(2) {
  [0]=>
  string(2) ".."
  [1]=>
  string(1) "."
}

-- Iteration 15 --
array(2) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
}

-- Iteration 16 --

Warning: scandir() expects parameter 2 to be int, string given in %s on line %d
NULL

-- Iteration 17 --

Warning: scandir() expects parameter 2 to be int, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: scandir() expects parameter 2 to be int, array given in %s on line %d
NULL

-- Iteration 19 --

Warning: scandir() expects parameter 2 to be int, string given in %s on line %d
NULL

-- Iteration 20 --

Warning: scandir() expects parameter 2 to be int, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: scandir() expects parameter 2 to be int, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: scandir() expects parameter 2 to be int, object given in %s on line %d
NULL

-- Iteration 23 --
array(2) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
}

-- Iteration 24 --
array(2) {
  [0]=>
  string(1) "."
  [1]=>
  string(2) ".."
}

-- Iteration 25 --

Warning: scandir() expects parameter 2 to be int, resource given in %s on line %d
NULL
===DONE===
