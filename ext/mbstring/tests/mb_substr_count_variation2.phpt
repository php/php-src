--TEST--
Test mb_substr_count() function : usage variations - pass different data types as $needle arg 
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_substr_count') or die("skip mb_substr_count() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_substr_count(string $haystack, string $needle [, string $encoding])
 * Description: Count the number of substring occurrences 
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Pass different data types as $needle to mb_substr_count() to test behaviour
 */

echo "*** Testing mb_substr_count() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$haystack = 'hello, world';

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
  public function __toString() {
    return "world";
  }
}

// heredoc string
$heredoc = <<<EOT
world
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $needle argument
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
/*18*/ "world",
       'world',
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

// loop through each element of $inputs to check the behavior of mb_substr_count()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( mb_substr_count($haystack, $input) );
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing mb_substr_count() : usage variations ***

-- Iteration 1 --
int(0)

-- Iteration 2 --
int(0)

-- Iteration 3 --
int(0)

-- Iteration 4 --
int(0)

-- Iteration 5 --
int(0)

-- Iteration 6 --
int(0)

-- Iteration 7 --
int(0)

-- Iteration 8 --
int(0)

-- Iteration 9 --
int(0)

-- Iteration 10 --

Warning: mb_substr_count(): Empty substring. in %s on line %d
bool(false)

-- Iteration 11 --

Warning: mb_substr_count(): Empty substring. in %s on line %d
bool(false)

-- Iteration 12 --
int(0)

-- Iteration 13 --

Warning: mb_substr_count(): Empty substring. in %s on line %d
bool(false)

-- Iteration 14 --
int(0)

-- Iteration 15 --

Warning: mb_substr_count(): Empty substring. in %s on line %d
bool(false)

-- Iteration 16 --

Warning: mb_substr_count(): Empty substring. in %s on line %d
bool(false)

-- Iteration 17 --

Warning: mb_substr_count(): Empty substring. in %s on line %d
bool(false)

-- Iteration 18 --
int(1)

-- Iteration 19 --
int(1)

-- Iteration 20 --
int(1)

-- Iteration 21 --
int(1)

-- Iteration 22 --

Warning: mb_substr_count(): Empty substring. in %s on line %d
bool(false)

-- Iteration 23 --

Warning: mb_substr_count(): Empty substring. in %s on line %d
bool(false)

-- Iteration 24 --

Warning: mb_substr_count() expects parameter 2 to be string, resource given in %s on line %d
NULL
Done