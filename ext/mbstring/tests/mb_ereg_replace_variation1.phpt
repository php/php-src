--TEST--
Test mb_ereg_replace() function : usage variations  - <type here specifics of this variation>
--INI--
error_reporting=E_ALL & ~E_NOTICE
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
$replacement = 'string_val';
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

);

// loop through each element of the array for pattern

$iterator = 1;
foreach($inputs as $input) {
      echo "\n-- Iteration $iterator --\n";
      var_dump( mb_ereg_replace($input, $replacement, $string, $option) );
      $iterator++;
};

echo "Done";
?>
--EXPECTF--
*** Testing mb_ereg_replace() : usage variations ***

-- Iteration 1 --
string(10) "string_val"

-- Iteration 2 --
string(10) "string_val"

-- Iteration 3 --
string(10) "string_val"

-- Iteration 4 --

Warning: mb_ereg_replace(): Pattern is not valid under UTF-8 encoding in %s on line %d
bool(false)

-- Iteration 5 --
string(10) "string_val"

-- Iteration 6 --

Warning: mb_ereg_replace(): Pattern is not valid under UTF-8 encoding in %s on line %d
bool(false)

-- Iteration 7 --
string(10) "string_val"

-- Iteration 8 --
string(10) "string_val"

-- Iteration 9 --
string(10) "string_val"

-- Iteration 10 --
string(10) "string_val"

-- Iteration 11 --
string(10) "string_val"

-- Iteration 12 --
string(10) "string_val"

-- Iteration 13 --
string(10) "string_val"

-- Iteration 14 --
string(10) "string_val"

-- Iteration 15 --
string(10) "string_val"

-- Iteration 16 --
string(120) "string_valsstring_valtstring_valrstring_valistring_valnstring_valgstring_val_string_valvstring_valastring_vallstring_val"

-- Iteration 17 --
string(120) "string_valsstring_valtstring_valrstring_valistring_valnstring_valgstring_val_string_valvstring_valastring_vallstring_val"

-- Iteration 18 --
string(10) "string_val"

-- Iteration 19 --
string(10) "string_val"

-- Iteration 20 --
string(10) "string_val"

-- Iteration 21 --
string(10) "string_val"

-- Iteration 22 --
string(10) "string_val"

-- Iteration 23 --
string(10) "string_val"
Done
