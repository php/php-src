--TEST--
Test mb_ereg_replace() function : usage variations
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_ereg_replace') or die("skip mb_ereg_replace() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : proto string mb_ereg_replace(string pattern, string replacement, string string [, string option])
 * Description: Replace regular expression for multibyte string 
 * Source code: ext/mbstring/php_mbregex.c
 * Alias to functions: 
 */

echo "*** Testing mb_ereg_replace() : usage variations ***\n";

// Initialise function arguments not being substituted (if any)
$pattern = '[a-z]';
$string = 'string_val';
$option = '';

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
      var_dump( mb_ereg_replace($pattern, $input, $string, $option) );
      $iterator++;
};
fclose($fp);
echo "Done";
?>
--EXPECTF--
*** Testing mb_ereg_replace() : usage variations ***

-- Iteration 1 --
string(10) "000000_000"

-- Iteration 2 --
string(10) "111111_111"

-- Iteration 3 --
string(46) "123451234512345123451234512345_123451234512345"

-- Iteration 4 --
string(46) "-2345-2345-2345-2345-2345-2345_-2345-2345-2345"

-- Iteration 5 --
string(37) "10.510.510.510.510.510.5_10.510.510.5"

-- Iteration 6 --
string(46) "-10.5-10.5-10.5-10.5-10.5-10.5_-10.5-10.5-10.5"

-- Iteration 7 --
string(109) "123456789000123456789000123456789000123456789000123456789000123456789000_123456789000123456789000123456789000"

-- Iteration 8 --
string(118) "1.23456789E-91.23456789E-91.23456789E-91.23456789E-91.23456789E-91.23456789E-9_1.23456789E-91.23456789E-91.23456789E-9"

-- Iteration 9 --
string(28) "0.50.50.50.50.50.5_0.50.50.5"

-- Iteration 10 --
string(1) "_"

-- Iteration 11 --
string(1) "_"

-- Iteration 12 --
string(10) "111111_111"

-- Iteration 13 --
string(1) "_"

-- Iteration 14 --
string(10) "111111_111"

-- Iteration 15 --
string(1) "_"

-- Iteration 16 --
string(1) "_"

-- Iteration 17 --
string(1) "_"

-- Iteration 18 --
string(46) "UTF-8UTF-8UTF-8UTF-8UTF-8UTF-8_UTF-8UTF-8UTF-8"

-- Iteration 19 --
string(46) "UTF-8UTF-8UTF-8UTF-8UTF-8UTF-8_UTF-8UTF-8UTF-8"

-- Iteration 20 --
string(46) "UTF-8UTF-8UTF-8UTF-8UTF-8UTF-8_UTF-8UTF-8UTF-8"

-- Iteration 21 --
string(46) "UTF-8UTF-8UTF-8UTF-8UTF-8UTF-8_UTF-8UTF-8UTF-8"

-- Iteration 22 --
string(1) "_"

-- Iteration 23 --
string(1) "_"

-- Iteration 24 --

Warning: mb_ereg_replace() expects parameter 2 to be string, resource given in %s on line %d
bool(false)
Done