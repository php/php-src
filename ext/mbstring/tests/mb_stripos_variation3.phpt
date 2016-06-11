--TEST--
Test mb_stripos() function : usage variations - pass different data types as $offset arg
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_stripos') or die("skip mb_stripos() is not available in this build");
if (PHP_INT_SIZE != 8) die('skip 64-bit only');
?>
--FILE--
<?php
/* Prototype  : int mb_stripos(string $haystack, string $needle [, int $offset [, string $encoding]])
 * Description: Find position of first occurrence of a string within another, case insensitive
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Pass mb_stripos different data types as $offset arg to test behaviour
 */

echo "*** Testing mb_stripos() : usage variations ***\n";

// Initialise function arguments not being substituted
$needle = b'A';
$haystack = b'string_val';
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
	   -5,
       -2345,

       // float data
/*6*/  10.5,
       -5.5,
	   -100.5,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // null data
/*12*/ NULL,
       null,

       // boolean data
/*14*/ true,
       false,
       TRUE,
       FALSE,
       
       // empty data
/*18*/ "",
       '',

       // string data
/*20*/ "string",
       'string',
       $heredoc,
       
       // object data
/*23*/ new classA(),

       // undefined data
/*24*/ @$undefined_var,

       // unset data
/*25*/ @$unset_var,

       // resource variable
/*26*/ $fp
);

// loop through each element of $inputs to check the behavior of mb_stripos()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( mb_stripos($haystack, $needle, $input, $encoding));
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing mb_stripos() : usage variations ***

-- Iteration 1 --
int(8)

-- Iteration 2 --
int(8)

-- Iteration 3 --

Warning: mb_stripos(): Offset not contained in string in %s on line %d
bool(false)

-- Iteration 4 --
int(8)

-- Iteration 5 --

Warning: mb_stripos(): Offset not contained in string in %s on line %d
bool(false)

-- Iteration 6 --
bool(false)

-- Iteration 7 --
int(8)

-- Iteration 8 --

Warning: mb_stripos(): Offset not contained in string in %s on line %d
bool(false)

-- Iteration 9 --

Warning: mb_stripos(): Offset not contained in string in %s on line %d
bool(false)

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
int(8)

-- Iteration 17 --
int(8)

-- Iteration 18 --

Warning: mb_stripos() expects parameter 3 to be integer, string given in %s on line %d
NULL

-- Iteration 19 --

Warning: mb_stripos() expects parameter 3 to be integer, string given in %s on line %d
NULL

-- Iteration 20 --

Warning: mb_stripos() expects parameter 3 to be integer, string given in %s on line %d
NULL

-- Iteration 21 --

Warning: mb_stripos() expects parameter 3 to be integer, string given in %s on line %d
NULL

-- Iteration 22 --

Warning: mb_stripos() expects parameter 3 to be integer, string given in %s on line %d
NULL

-- Iteration 23 --

Warning: mb_stripos() expects parameter 3 to be integer, object given in %s on line %d
NULL

-- Iteration 24 --
int(8)

-- Iteration 25 --
int(8)

-- Iteration 26 --

Warning: mb_stripos() expects parameter 3 to be integer, resource given in %s on line %d
NULL
Done
