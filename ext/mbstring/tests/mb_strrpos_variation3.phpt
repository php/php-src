--TEST--
Test mb_strrpos() function : usage variations - Pass different data types as $offset arg
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strrpos') or die("skip mb_strrpos() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_strrpos(string $haystack, string $needle [, int $offset [, string $encoding]])
 * Description: Find position of last occurrence of a string within another 
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Pass mb_strrpos() different data types as $offset argument to test behaviour
 */

echo "*** Testing mb_strrpos() : usage variations ***\n";

// Initialise function arguments not being substituted
$needle = b'a';
$haystack = b'string_val';
$encoding = 'utf-8';

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
  public function __toString() {
    return b"7";
  }
}

// heredoc string
$heredoc = b<<<EOT
hello world
EOT;

// unexpected values to be passed to $offset argument
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
       
       // object data
/*16*/ new classA(),

       // undefined data
/*17*/ @$undefined_var,

       // unset data
/*18*/ @$unset_var
);

// loop through each element of $inputs to check the behavior of mb_strrpos()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( mb_strrpos($haystack, $needle, $input, $encoding));
  $iterator++;
};

echo "Done";
?>
--EXPECTF--
*** Testing mb_strrpos() : usage variations ***

-- Iteration 1 --
int(8)

-- Iteration 2 --
int(8)

-- Iteration 3 --

Notice: mb_strrpos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)

-- Iteration 4 --

Notice: mb_strrpos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)

-- Iteration 5 --
bool(false)

-- Iteration 6 --
bool(false)

-- Iteration 7 --

Notice: mb_strrpos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)

-- Iteration 8 --
int(8)

-- Iteration 9 --
int(8)

-- Iteration 10 --
int(8)

-- Iteration 11 --
int(8)

-- Iteration 12 --
int(8)

-- Iteration 13 --
int(8)

-- Iteration 14 --
int(8)

-- Iteration 15 --
int(8)

-- Iteration 16 --

Notice: Object of class classA could not be converted to int in %s on line %d
int(8)

-- Iteration 17 --
int(8)

-- Iteration 18 --
int(8)
Done