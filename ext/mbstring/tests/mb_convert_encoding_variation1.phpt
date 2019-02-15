--TEST--
Test mb_convert_encoding() function : usage variations - pass different data types as $to_encoding arg
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_convert_encoding') or die("skip mb_convert_encoding() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_convert_encoding(string $str, string $to_encoding [, mixed $from_encoding])
 * Description: Returns converted string in desired encoding
 * Source code: ext/mbstring/mbstring.c
 */


/*
 * Pass different data types to $to_encoding arg to test behaviour of mb_convert_encoding
 */

echo "*** Testing mb_convert_encoding() : usage variations ***\n";

// Initialise function arguments not being substituted
mb_internal_encoding('utf-8');
$sourcestring = 'hello, world';

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
UTF-8
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// unexpected values to be passed to $to_encoding argument
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

// loop through each element of $inputs to check the behaviour of mb_convert_encoding()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump(bin2hex( mb_convert_encoding($sourcestring, $input, 'ISO-8859-1') ));
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing mb_convert_encoding() : usage variations ***

-- Iteration 1 --

Warning: mb_convert_encoding(): Unknown encoding "0" in %s on line %d
string(0) ""

-- Iteration 2 --

Warning: mb_convert_encoding(): Unknown encoding "1" in %s on line %d
string(0) ""

-- Iteration 3 --

Warning: mb_convert_encoding(): Unknown encoding "12345" in %s on line %d
string(0) ""

-- Iteration 4 --

Warning: mb_convert_encoding(): Unknown encoding "-2345" in %s on line %d
string(0) ""

-- Iteration 5 --

Warning: mb_convert_encoding(): Unknown encoding "10.5" in %s on line %d
string(0) ""

-- Iteration 6 --

Warning: mb_convert_encoding(): Unknown encoding "-10.5" in %s on line %d
string(0) ""

-- Iteration 7 --

Warning: mb_convert_encoding(): Unknown encoding "123456789000" in %s on line %d
string(0) ""

-- Iteration 8 --

Warning: mb_convert_encoding(): Unknown encoding "1.23456789E-9" in %s on line %d
string(0) ""

-- Iteration 9 --

Warning: mb_convert_encoding(): Unknown encoding "0.5" in %s on line %d
string(0) ""

-- Iteration 10 --
string(24) "68656c6c6f2c20776f726c64"

-- Iteration 11 --
string(24) "68656c6c6f2c20776f726c64"

-- Iteration 12 --

Warning: mb_convert_encoding(): Unknown encoding "1" in %s on line %d
string(0) ""

-- Iteration 13 --
string(24) "68656c6c6f2c20776f726c64"

-- Iteration 14 --

Warning: mb_convert_encoding(): Unknown encoding "1" in %s on line %d
string(0) ""

-- Iteration 15 --
string(24) "68656c6c6f2c20776f726c64"

-- Iteration 16 --
string(24) "68656c6c6f2c20776f726c64"

-- Iteration 17 --
string(24) "68656c6c6f2c20776f726c64"

-- Iteration 18 --
string(24) "68656c6c6f2c20776f726c64"

-- Iteration 19 --
string(24) "68656c6c6f2c20776f726c64"

-- Iteration 20 --
string(24) "68656c6c6f2c20776f726c64"

-- Iteration 21 --

Warning: mb_convert_encoding(): Unknown encoding "Class A object" in %s on line %d
string(0) ""

-- Iteration 22 --
string(24) "68656c6c6f2c20776f726c64"

-- Iteration 23 --
string(24) "68656c6c6f2c20776f726c64"

-- Iteration 24 --

Warning: mb_convert_encoding() expects parameter 2 to be string, resource given in %s on line %d
string(0) ""
Done
