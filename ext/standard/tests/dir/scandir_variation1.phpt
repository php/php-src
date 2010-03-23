--TEST--
Test scandir() function : usage variations - different data types as $dir arg
--FILE--
<?php
/* Prototype  : array scandir(string $dir [, int $sorting_order [, resource $context]])
 * Description: List files & directories inside the specified path 
 * Source code: ext/standard/dir.c
 */

/*
 * Pass different data types as $dir argument to test behaviour of scandir()
 */

echo "*** Testing scandir() : usage variations ***\n";

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

// unexpected values to be passed to $dir argument
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
  var_dump( scandir($input) );
  $iterator++;
};

fclose($fp);
?>
===DONE===
--EXPECTF--
*** Testing scandir() : usage variations ***

-- Iteration 1 --

Warning: scandir(0): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

-- Iteration 2 --

Warning: scandir(1): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

-- Iteration 3 --

Warning: scandir(12345): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

-- Iteration 4 --

Warning: scandir(-2345): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

-- Iteration 5 --

Warning: scandir(10.5): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

-- Iteration 6 --

Warning: scandir(-10.5): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

-- Iteration 7 --

Warning: scandir(123456789000): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

-- Iteration 8 --

Warning: scandir(1.23456789E-9): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

-- Iteration 9 --

Warning: scandir(0.5): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

-- Iteration 10 --

Warning: scandir(): Directory name cannot be empty in %s on line %d
bool(false)

-- Iteration 11 --

Warning: scandir(): Directory name cannot be empty in %s on line %d
bool(false)

-- Iteration 12 --

Warning: scandir(1): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

-- Iteration 13 --

Warning: scandir(): Directory name cannot be empty in %s on line %d
bool(false)

-- Iteration 14 --

Warning: scandir(1): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

-- Iteration 15 --

Warning: scandir(): Directory name cannot be empty in %s on line %d
bool(false)

-- Iteration 16 --

Warning: scandir(): Directory name cannot be empty in %s on line %d
bool(false)

-- Iteration 17 --

Warning: scandir(): Directory name cannot be empty in %s on line %d
bool(false)

-- Iteration 18 --

Warning: scandir() expects parameter 1 to be string, array given in %s on line %d
NULL

-- Iteration 19 --

Warning: scandir(string): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

-- Iteration 20 --

Warning: scandir(string): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

-- Iteration 21 --

Warning: scandir(hello world): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

-- Iteration 22 --

Warning: scandir(Class A object): failed to open dir: %s in %s on line %d

Warning: scandir(): (errno %d): %s in %s on line %d
bool(false)

-- Iteration 23 --

Warning: scandir(): Directory name cannot be empty in %s on line %d
bool(false)

-- Iteration 24 --

Warning: scandir(): Directory name cannot be empty in %s on line %d
bool(false)

-- Iteration 25 --

Warning: scandir() expects parameter 1 to be string, resource given in %s on line %d
NULL
===DONE===
