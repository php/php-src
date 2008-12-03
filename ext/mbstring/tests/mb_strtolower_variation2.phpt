--TEST--
Test mb_strtolower() function : usage variations - pass different data types as $encoding arg
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strtolower') or die("skip mb_strtolower() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_strtolower(string $sourcestring [, string $encoding])
 * Description: Returns a lowercased version of $sourcestring
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Pass different data types to $encoding argument to test behaviour of mb_strtolower()
 * Where possible, 'UTF-8' has been entered as a string value
 */

echo "*** Testing mb_strtolower() : usage variations ***\n";

// Initialise function arguments not being substituted
$sourcestring = b'Hello, World';

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

// loop through each element of $inputs to check the behavior of mb_strtolower()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  $res = mb_strtolower($sourcestring, $input);
  if ($res === false || $res == NULL) {
     var_dump($res);
  }
  else {
     var_dump(bin2hex($res));
  }
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing mb_strtolower() : usage variations ***

-- Iteration 1 --

Warning: mb_strtolower(): Unknown encoding "0" in %s on line %d
bool(false)

-- Iteration 2 --

Warning: mb_strtolower(): Unknown encoding "1" in %s on line %d
bool(false)

-- Iteration 3 --

Warning: mb_strtolower(): Unknown encoding "12345" in %s on line %d
bool(false)

-- Iteration 4 --

Warning: mb_strtolower(): Unknown encoding "-2345" in %s on line %d
bool(false)

-- Iteration 5 --

Warning: mb_strtolower(): Unknown encoding "10.5" in %s on line %d
bool(false)

-- Iteration 6 --

Warning: mb_strtolower(): Unknown encoding "-10.5" in %s on line %d
bool(false)

-- Iteration 7 --

Warning: mb_strtolower(): Unknown encoding "123456789000" in %s on line %d
bool(false)

-- Iteration 8 --

Warning: mb_strtolower(): Unknown encoding "1.23456789E-9" in %s on line %d
bool(false)

-- Iteration 9 --

Warning: mb_strtolower(): Unknown encoding "0.5" in %s on line %d
bool(false)

-- Iteration 10 --

Warning: mb_strtolower(): Unknown encoding "(null)" in %s on line %d
bool(false)

-- Iteration 11 --

Warning: mb_strtolower(): Unknown encoding "(null)" in %s on line %d
bool(false)

-- Iteration 12 --

Warning: mb_strtolower(): Unknown encoding "1" in %s on line %d
bool(false)

-- Iteration 13 --

Warning: mb_strtolower(): Unknown encoding "" in %s on line %d
bool(false)

-- Iteration 14 --

Warning: mb_strtolower(): Unknown encoding "1" in %s on line %d
bool(false)

-- Iteration 15 --

Warning: mb_strtolower(): Unknown encoding "" in %s on line %d
bool(false)

-- Iteration 16 --

Warning: mb_strtolower(): Unknown encoding "" in %s on line %d
bool(false)

-- Iteration 17 --

Warning: mb_strtolower(): Unknown encoding "" in %s on line %d
bool(false)

-- Iteration 18 --
string(24) "68656c6c6f2c20776f726c64"

-- Iteration 19 --
string(24) "68656c6c6f2c20776f726c64"

-- Iteration 20 --
string(24) "68656c6c6f2c20776f726c64"

-- Iteration 21 --
string(24) "68656c6c6f2c20776f726c64"

-- Iteration 22 --

Warning: mb_strtolower(): Unknown encoding "(null)" in %s on line %d
bool(false)

-- Iteration 23 --

Warning: mb_strtolower(): Unknown encoding "(null)" in %s on line %d
bool(false)

-- Iteration 24 --

Warning: mb_strtolower() expects parameter 2 to be string, resource given in %s on line %d
NULL
Done