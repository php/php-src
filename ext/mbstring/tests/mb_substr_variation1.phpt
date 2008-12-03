--TEST--
Test mb_substr() function : usage variations - pass unexpected arguments (including strings) in place of $str
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_substr') or die("skip mb_substr() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_substr(string $str, int $start [, int $length [, string $encoding]])
 * Description: Returns part of a string 
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Pass different data types as $str to mb_substr() to test behaviour
 */

echo "*** Testing mb_substr() : usage variations ***\n";

// Initialise function arguments not being substituted
$start = 0;
$length = 5;

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

// loop through each element of $inputs to check the behavior of mb_substr()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( mb_substr($input, $start, $length));
  $iterator++;
};

fclose($fp);
echo "Done";
?>
--EXPECTF--
*** Testing mb_substr() : usage variations ***

-- Iteration 1 --
string(1) "0"

-- Iteration 2 --
string(1) "1"

-- Iteration 3 --
string(5) "12345"

-- Iteration 4 --
string(5) "-2345"

-- Iteration 5 --
string(4) "10.5"

-- Iteration 6 --
string(5) "-10.5"

-- Iteration 7 --
string(5) "12345"

-- Iteration 8 --
string(5) "1.234"

-- Iteration 9 --
string(3) "0.5"

-- Iteration 10 --
string(0) ""

-- Iteration 11 --
string(0) ""

-- Iteration 12 --
string(1) "1"

-- Iteration 13 --
string(0) ""

-- Iteration 14 --
string(1) "1"

-- Iteration 15 --
string(0) ""

-- Iteration 16 --
string(0) ""

-- Iteration 17 --
string(0) ""

-- Iteration 18 --
string(5) "strin"

-- Iteration 19 --
string(5) "strin"

-- Iteration 20 --
string(5) "hello"

-- Iteration 21 --
string(5) "Class"

-- Iteration 22 --
string(0) ""

-- Iteration 23 --
string(0) ""

-- Iteration 24 --
string(5) "Resou"
Done