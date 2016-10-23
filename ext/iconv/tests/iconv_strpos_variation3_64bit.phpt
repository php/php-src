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
       0,
       1,
       12345,
	   -5,
       -2345,

       // float data
       10.5,
       -9.5,
       -100.3,
       12.3456789000e10,
       12.3456789000E-10,
       .5,

       // null data
       NULL,
       null,

       // boolean data
       true,
       false,
       TRUE,
       FALSE,
       
       // empty data
       "",
       '',

       // string data
       "string",
       'string',
       $heredoc,
       
       // object data
       new classA(),

       // undefined data
       @$undefined_var,

       // unset data
       @$unset_var,

       // resource variable
       $fp
);

// loop through each element of $inputs to check the behavior of iconv_strpos()

foreach($inputs as $input) {
  echo "--\n";
  var_dump($input);
  var_dump( iconv_strpos($haystack, $needle, $input, $encoding));
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing iconv_strpos() : usage variations ***
--
int(0)
int(8)
--
int(1)
int(8)
--
int(12345)
bool(false)
--
int(-5)
int(8)
--
int(-2345)

Warning: iconv_strpos(): Offset not contained in string. in %s on line %d
bool(false)
--
float(10.5)
bool(false)
--
float(-9.5)
int(8)
--
float(-100.3)

Warning: iconv_strpos(): Offset not contained in string. in %s on line %d
bool(false)
--
float(123456789000)
bool(false)
--
float(1.23456789E-9)
int(8)
--
float(0.5)
int(8)
--
NULL
int(8)
--
NULL
int(8)
--
bool(true)
int(8)
--
bool(false)
int(8)
--
bool(true)
int(8)
--
bool(false)
int(8)
--
string(0) ""

Warning: iconv_strpos() expects parameter 3 to be integer, string given in %s on line %d
bool(false)
--
string(0) ""

Warning: iconv_strpos() expects parameter 3 to be integer, string given in %s on line %d
bool(false)
--
string(6) "string"

Warning: iconv_strpos() expects parameter 3 to be integer, string given in %s on line %d
bool(false)
--
string(6) "string"

Warning: iconv_strpos() expects parameter 3 to be integer, string given in %s on line %d
bool(false)
--
string(11) "hello world"

Warning: iconv_strpos() expects parameter 3 to be integer, string given in %s on line %d
bool(false)
--
object(classA)#%d (%d) {
}

Warning: iconv_strpos() expects parameter 3 to be integer, object given in %s on line %d
bool(false)
--
NULL
int(8)
--
NULL
int(8)
--
resource(%d) of type (stream)

Warning: iconv_strpos() expects parameter 3 to be integer, resource given in %s on line %d
bool(false)
Done
