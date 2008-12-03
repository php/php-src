--TEST--
Test mb_strrpos() function : usage variations - pass different data types to $haystack arg
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
 * Pass mb_strrpos() different data types as $haystack argument to test behaviour
 */

echo "*** Testing mb_strrpos() : usage variations ***\n";

// Initialise function arguments not being substituted
$needle = b'world';
$offset = 0;
$encoding = 'utf-8';

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
  public function __toString() {
    return b"hello, world";
  }
}

// heredoc string
$heredoc = b<<<EOT
hello, world
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $haystack argument
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
/*18*/ b"hello, world",
       b'hello, world',
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

// loop through each element of $inputs to check the behavior of mb_strrpos()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( mb_strrpos($input, $needle, $offset, $encoding));
  $iterator++;
};

echo "Done";
?>

--EXPECTF--
*** Testing mb_strrpos() : usage variations ***

-- Iteration 1 --
bool(false)

-- Iteration 2 --
bool(false)

-- Iteration 3 --
bool(false)

-- Iteration 4 --
bool(false)

-- Iteration 5 --
bool(false)

-- Iteration 6 --
bool(false)

-- Iteration 7 --
bool(false)

-- Iteration 8 --
bool(false)

-- Iteration 9 --
bool(false)

-- Iteration 10 --
bool(false)

-- Iteration 11 --
bool(false)

-- Iteration 12 --
bool(false)

-- Iteration 13 --
bool(false)

-- Iteration 14 --
bool(false)

-- Iteration 15 --
bool(false)

-- Iteration 16 --
bool(false)

-- Iteration 17 --
bool(false)

-- Iteration 18 --
int(7)

-- Iteration 19 --
int(7)

-- Iteration 20 --
int(7)

-- Iteration 21 --
int(7)

-- Iteration 22 --
bool(false)

-- Iteration 23 --
bool(false)

-- Iteration 24 --

Warning: mb_strrpos() expects parameter 1 to be string, resource given in %s on line %d
bool(false)
Done