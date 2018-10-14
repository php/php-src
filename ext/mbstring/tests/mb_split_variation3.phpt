--TEST--
Test mb_split() function : usage variations  - different parameter types for limit
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_split') or die("skip mb_split() is not available in this build");
if (PHP_INT_SIZE != 8) die('skip 64-bit only');
?>
--FILE--
<?php
/* Prototype  : proto array mb_split(string pattern, string string [, int limit])
 * Description: split multibyte string into array by regular expression
 * Source code: ext/mbstring/php_mbregex.c
 * Alias to functions:
 */

echo "*** Testing mb_split() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$pattern = '[a-z]';
$string = 'string_val';

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// get a class
class classA
{
  public function __toString() {
    return "UTF-8";
  }
}

// heredoc string
$heredoc = <<<EOT
UTF-8
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $encoding argument
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
/*18*/ "UTF-8",
       'UTF-8',
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

// loop through each element of the array for pattern

$iterator = 1;
foreach($inputs as $input) {
      echo "\n-- Iteration $iterator --\n";
      var_dump( mb_split($pattern, $string, $input) );
      $iterator++;
};

fclose($fp);
echo "Done";
?>
--EXPECTF--
*** Testing mb_split() : usage variations ***

-- Iteration 1 --
array(1) {
  [0]=>
  string(10) "string_val"
}

-- Iteration 2 --
array(1) {
  [0]=>
  string(10) "string_val"
}

-- Iteration 3 --
array(10) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
  [4]=>
  string(0) ""
  [5]=>
  string(0) ""
  [6]=>
  string(1) "_"
  [7]=>
  string(0) ""
  [8]=>
  string(0) ""
  [9]=>
  string(0) ""
}

-- Iteration 4 --
array(10) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
  [4]=>
  string(0) ""
  [5]=>
  string(0) ""
  [6]=>
  string(1) "_"
  [7]=>
  string(0) ""
  [8]=>
  string(0) ""
  [9]=>
  string(0) ""
}

-- Iteration 5 --
array(10) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
  [4]=>
  string(0) ""
  [5]=>
  string(0) ""
  [6]=>
  string(1) "_"
  [7]=>
  string(0) ""
  [8]=>
  string(0) ""
  [9]=>
  string(0) ""
}

-- Iteration 6 --
array(10) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
  [4]=>
  string(0) ""
  [5]=>
  string(0) ""
  [6]=>
  string(1) "_"
  [7]=>
  string(0) ""
  [8]=>
  string(0) ""
  [9]=>
  string(0) ""
}

-- Iteration 7 --
array(10) {
  [0]=>
  string(0) ""
  [1]=>
  string(0) ""
  [2]=>
  string(0) ""
  [3]=>
  string(0) ""
  [4]=>
  string(0) ""
  [5]=>
  string(0) ""
  [6]=>
  string(1) "_"
  [7]=>
  string(0) ""
  [8]=>
  string(0) ""
  [9]=>
  string(0) ""
}

-- Iteration 8 --
array(1) {
  [0]=>
  string(10) "string_val"
}

-- Iteration 9 --
array(1) {
  [0]=>
  string(10) "string_val"
}

-- Iteration 10 --
array(1) {
  [0]=>
  string(10) "string_val"
}

-- Iteration 11 --
array(1) {
  [0]=>
  string(10) "string_val"
}

-- Iteration 12 --
array(1) {
  [0]=>
  string(10) "string_val"
}

-- Iteration 13 --
array(1) {
  [0]=>
  string(10) "string_val"
}

-- Iteration 14 --
array(1) {
  [0]=>
  string(10) "string_val"
}

-- Iteration 15 --
array(1) {
  [0]=>
  string(10) "string_val"
}

-- Iteration 16 --

Warning: mb_split() expects parameter 3 to be int, string given in %s on line %d
bool(false)

-- Iteration 17 --

Warning: mb_split() expects parameter 3 to be int, string given in %s on line %d
bool(false)

-- Iteration 18 --

Warning: mb_split() expects parameter 3 to be int, string given in %s on line %d
bool(false)

-- Iteration 19 --

Warning: mb_split() expects parameter 3 to be int, string given in %s on line %d
bool(false)

-- Iteration 20 --

Warning: mb_split() expects parameter 3 to be int, string given in %s on line %d
bool(false)

-- Iteration 21 --

Warning: mb_split() expects parameter 3 to be int, object given in %s on line %d
bool(false)

-- Iteration 22 --
array(1) {
  [0]=>
  string(10) "string_val"
}

-- Iteration 23 --
array(1) {
  [0]=>
  string(10) "string_val"
}

-- Iteration 24 --

Warning: mb_split() expects parameter 3 to be int, resource given in %s on line %d
bool(false)
Done
