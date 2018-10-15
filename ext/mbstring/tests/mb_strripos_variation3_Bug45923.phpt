--TEST--
Test mb_strripos() function : usage variations - pass different data types as $offset arg
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strripos') or die("skip mb_strripos() is not available in this build");
if (PHP_INT_SIZE != 8) die('skip 64-bit only');
?>
--FILE--
<?php
/* Prototype  : int mb_strripos(string haystack, string needle [, int offset [, string encoding]])
 * Description: Finds position of last occurrence of a string within another, case insensitive
 * Source code: ext/mbstring/mbstring.c
 * Alias to functions:
 */

/*
 * Pass mb_strripos different data types as $offset arg to test behaviour
 */

echo "*** Testing mb_strripos() : usage variations ***\n";

// Initialise function arguments not being substituted
$needle = 'A';
$haystack = 'string_val';
$encoding = 'utf-8';

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

// unexpected values to be passed to $offest argument
$inputs = array(

       // int data
/*1*/  0,
       1,
       12345,
       -2345,

       // float data
/*5*/  12.5,
       -12.5,
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

       // object data
/*21*/ new classA(),

       // undefined data
/*22*/ @$undefined_var,

       // unset data
/*23*/ @$unset_var,

       // resource variable
/*24*/ $fp
);

// loop through each element of $inputs to check the behavior of mb_strripos()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( mb_strripos($haystack, $needle, $input, $encoding));
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing mb_strripos() : usage variations ***

-- Iteration 1 --
int(8)

-- Iteration 2 --
int(8)

-- Iteration 3 --

Warning: mb_strripos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)

-- Iteration 4 --

Warning: mb_strripos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)

-- Iteration 5 --

Warning: mb_strripos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)

-- Iteration 6 --

Warning: mb_strripos(): Offset is greater than the length of haystack string in %s on line %d
bool(false)

-- Iteration 7 --

Warning: mb_strripos(): Offset is greater than the length of haystack string in %s on line %d
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

Warning: mb_strripos() expects parameter 3 to be integer, string given in %s on line %d
NULL

-- Iteration 17 --

Warning: mb_strripos() expects parameter 3 to be integer, string given in %s on line %d
NULL

-- Iteration 18 --

Warning: mb_strripos() expects parameter 3 to be integer, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: mb_strripos() expects parameter 3 to be integer, string given in %s on line %d
NULL

-- Iteration 20 --

Warning: mb_strripos() expects parameter 3 to be integer, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: mb_strripos() expects parameter 3 to be integer, object given in %s on line %d
NULL

-- Iteration 22 --
int(8)

-- Iteration 23 --
int(8)

-- Iteration 24 --

Warning: mb_strripos() expects parameter 3 to be integer, resource given in %s on line %d
NULL
Done
