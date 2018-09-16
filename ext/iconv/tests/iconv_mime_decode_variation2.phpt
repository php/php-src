--TEST--
Test iconv_mime_decode() function : usage variations - Pass different data types to mode arg
--SKIPIF--
<?php
PHP_INT_SIZE == 4 or die('skip 32-bit only');
extension_loaded('iconv') or die('skip iconv extension not loaded');
function_exists('iconv_mime_decode') or die("skip iconv_mime_decode() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string iconv_mime_decode(string encoded_string [, int mode, string charset])
 * Description: Decodes a mime header field
 * Source code: ext/iconv/iconv.c
 */

/*
 * Pass different data types to $str argument to see how iconv_mime_decode() behaves
 */

echo "*** Testing iconv_mime_decode() : usage variations ***\n";

// Initialise function arguments not being substituted
$header = 'Subject: =?UTF-8?B?UHLDvGZ1bmcgUHLDvGZ1bmc=?=';
$mode = ICONV_MIME_DECODE_CONTINUE_ON_ERROR;
$charset = 'UTF-8';


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

// loop through each element of $inputs to check the behavior of iconv_mime_decode()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( bin2hex(iconv_mime_decode($header, $input, $charset)));
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing iconv_mime_decode() : usage variations ***

-- Iteration 1 --
string(52) "5375626a6563743a205072c3bc66756e67205072c3bc66756e67"

-- Iteration 2 --
string(52) "5375626a6563743a205072c3bc66756e67205072c3bc66756e67"

-- Iteration 3 --
string(52) "5375626a6563743a205072c3bc66756e67205072c3bc66756e67"

-- Iteration 4 --
string(52) "5375626a6563743a205072c3bc66756e67205072c3bc66756e67"

-- Iteration 5 --
string(52) "5375626a6563743a205072c3bc66756e67205072c3bc66756e67"

-- Iteration 6 --
string(52) "5375626a6563743a205072c3bc66756e67205072c3bc66756e67"

-- Iteration 7 --

Warning: iconv_mime_decode() expects parameter 2 to be int, float given in %s on line %d
string(0) ""

-- Iteration 8 --
string(52) "5375626a6563743a205072c3bc66756e67205072c3bc66756e67"

-- Iteration 9 --
string(52) "5375626a6563743a205072c3bc66756e67205072c3bc66756e67"

-- Iteration 10 --
string(52) "5375626a6563743a205072c3bc66756e67205072c3bc66756e67"

-- Iteration 11 --
string(52) "5375626a6563743a205072c3bc66756e67205072c3bc66756e67"

-- Iteration 12 --
string(52) "5375626a6563743a205072c3bc66756e67205072c3bc66756e67"

-- Iteration 13 --
string(52) "5375626a6563743a205072c3bc66756e67205072c3bc66756e67"

-- Iteration 14 --
string(52) "5375626a6563743a205072c3bc66756e67205072c3bc66756e67"

-- Iteration 15 --
string(52) "5375626a6563743a205072c3bc66756e67205072c3bc66756e67"

-- Iteration 16 --

Warning: iconv_mime_decode() expects parameter 2 to be int, string given in %s on line %d
string(0) ""

-- Iteration 17 --

Warning: iconv_mime_decode() expects parameter 2 to be int, string given in %s on line %d
string(0) ""

-- Iteration 18 --

Warning: iconv_mime_decode() expects parameter 2 to be int, string given in %s on line %d
string(0) ""

-- Iteration 19 --

Warning: iconv_mime_decode() expects parameter 2 to be int, string given in %s on line %d
string(0) ""

-- Iteration 20 --

Warning: iconv_mime_decode() expects parameter 2 to be int, string given in %s on line %d
string(0) ""

-- Iteration 21 --

Warning: iconv_mime_decode() expects parameter 2 to be int, object given in %s on line %d
string(0) ""

-- Iteration 22 --
string(52) "5375626a6563743a205072c3bc66756e67205072c3bc66756e67"

-- Iteration 23 --
string(52) "5375626a6563743a205072c3bc66756e67205072c3bc66756e67"

-- Iteration 24 --

Warning: iconv_mime_decode() expects parameter 2 to be int, resource given in %s on line %d
string(0) ""
Done
