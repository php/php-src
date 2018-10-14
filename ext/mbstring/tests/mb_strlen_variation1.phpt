--TEST--
Test mb_strlen() function : usage variations - Pass different data types as $str arg
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strlen') or die("skip mb_strlen() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int mb_strlen(string $str [, string $encoding])
 * Description: Get character numbers of a string
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Test mb_strlen by passing different data types as $str argument
 */

echo "*** Testing mb_strlen() : usage variations ***\n";

// Initialise function arguments not being substituted
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

// unexpected values to be passed to $str argument
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

// loop through each element of $inputs to check the behavior of mb_strlen()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( mb_strlen($input, $encoding));
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing mb_strlen() : usage variations ***

-- Iteration 1 --
int(1)

-- Iteration 2 --
int(1)

-- Iteration 3 --
int(5)

-- Iteration 4 --
int(5)

-- Iteration 5 --
int(4)

-- Iteration 6 --
int(5)

-- Iteration 7 --
int(12)

-- Iteration 8 --
int(13)

-- Iteration 9 --
int(3)

-- Iteration 10 --
int(0)

-- Iteration 11 --
int(0)

-- Iteration 12 --
int(1)

-- Iteration 13 --
int(0)

-- Iteration 14 --
int(1)

-- Iteration 15 --
int(0)

-- Iteration 16 --
int(0)

-- Iteration 17 --
int(0)

-- Iteration 18 --
int(6)

-- Iteration 19 --
int(6)

-- Iteration 20 --
int(11)

-- Iteration 21 --
int(14)

-- Iteration 22 --
int(0)

-- Iteration 23 --
int(0)

-- Iteration 24 --

Warning: mb_strlen() expects parameter 1 to be string, resource given in %s on line %d
NULL
Done
