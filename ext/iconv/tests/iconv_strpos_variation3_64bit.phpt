--TEST--
Test iconv_strpos() function : usage variations - pass different data types as $offset arg
--SKIPIF--
<?php
extension_loaded('iconv') or die('skip');
function_exists('iconv_strpos') or die("skip iconv_strpos() is not available in this build");
if (PHP_INT_SIZE != 8) die("skip this test is for 64bit platform only");
?>
--FILE--
<?php
/* Prototype  : int iconv_strpos(string haystack, string needle [, int offset [, string charset]])
 * Description: Find position of first occurrence of a string within another 
 * Source code: ext/iconv/iconv.c
 */

/*
 * Pass iconv_strpos different data types as $offset arg to test behaviour
 */

echo "*** Testing iconv_strpos() : usage variations ***\n";

// Initialise function arguments not being substituted
$needle = b'a';
$haystack = b'string_val';
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

// unexpected values to be passed to $offest argument
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

// loop through each element of $inputs to check the behavior of iconv_strpos()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( iconv_strpos($haystack, $needle, $input, $encoding));
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing iconv_strpos() : usage variations ***

-- Iteration 1 --
int(8)

-- Iteration 2 --
int(8)

-- Iteration 3 --
bool(false)

-- Iteration 4 --

Warning: iconv_strpos(): Offset not contained in string. in %s on line %d
bool(false)

-- Iteration 5 --
bool(false)

-- Iteration 6 --

Warning: iconv_strpos(): Offset not contained in string. in %s on line %d
bool(false)

-- Iteration 7 --
int(8)

-- Iteration 8 --
int(8)

-- Iteration 9 --
int(8)

-- Iteration 10 --
int(8)

-- Iteration 11 --
int(8)

-- Iteration 12 --
int(8)

-- Iteration 13 --
int(8)

-- Iteration 14 --
int(8)

-- Iteration 15 --
int(8)

-- Iteration 16 --

Warning: iconv_strpos() expects parameter 3 to be long, string given in %s on line %d
bool(false)

-- Iteration 17 --

Warning: iconv_strpos() expects parameter 3 to be long, string given in %s on line %d
bool(false)

-- Iteration 18 --

Warning: iconv_strpos() expects parameter 3 to be long, string given in %s on line %d
bool(false)

-- Iteration 19 --

Warning: iconv_strpos() expects parameter 3 to be long, string given in %s on line %d
bool(false)

-- Iteration 20 --

Warning: iconv_strpos() expects parameter 3 to be long, string given in %s on line %d
bool(false)

-- Iteration 21 --

Warning: iconv_strpos() expects parameter 3 to be long, object given in %s on line %d
bool(false)

-- Iteration 22 --
int(8)

-- Iteration 23 --
int(8)

-- Iteration 24 --

Warning: iconv_strpos() expects parameter 3 to be long, resource given in %s on line %d
bool(false)
Done