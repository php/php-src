--TEST--
Test iconv_set_encoding() function : error functionality
--SKIPIF--
<?php
extension_loaded('iconv') or die('skip');
function_exists('iconv_set_encoding') or die("skip iconv_set_encoding() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : bool iconv_set_encoding(string type, string charset)
 * Description: Sets internal encoding and output encoding for ob_iconv_handler()
 * Source code: ext/iconv/iconv.c 
 */

/*
 * Test Error functionality of iconv_get_encoding
 */

echo "*** Testing iconv_set_encoding() : error functionality ***\n";

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
Nothing
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

       // invalid string data
/*18*/ "Nothing",
       'Nothing',
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

// loop through each element of $inputs to check the behavior of mb_regex_encoding()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( iconv_set_encoding("internal_encoding", $input) );
  var_dump( iconv_set_encoding("input_encoding", $input) );
  var_dump( iconv_set_encoding("output_encoding", $input) );
  var_dump( iconv_get_encoding("internal_encoding") );
  var_dump( iconv_get_encoding("input_encoding") );
  var_dump( iconv_get_encoding("output_encoding") );

  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing iconv_set_encoding() : error functionality ***

-- Iteration 1 --
bool(true)
bool(true)
bool(true)
string(1) "0"
string(1) "0"
string(1) "0"

-- Iteration 2 --
bool(true)
bool(true)
bool(true)
string(1) "1"
string(1) "1"
string(1) "1"

-- Iteration 3 --
bool(true)
bool(true)
bool(true)
string(5) "12345"
string(5) "12345"
string(5) "12345"

-- Iteration 4 --
bool(true)
bool(true)
bool(true)
string(5) "-2345"
string(5) "-2345"
string(5) "-2345"

-- Iteration 5 --
bool(true)
bool(true)
bool(true)
string(4) "10.5"
string(4) "10.5"
string(4) "10.5"

-- Iteration 6 --
bool(true)
bool(true)
bool(true)
string(5) "-10.5"
string(5) "-10.5"
string(5) "-10.5"

-- Iteration 7 --
bool(true)
bool(true)
bool(true)
string(12) "123456789000"
string(12) "123456789000"
string(12) "123456789000"

-- Iteration 8 --
bool(true)
bool(true)
bool(true)
string(13) "1.23456789E-9"
string(13) "1.23456789E-9"
string(13) "1.23456789E-9"

-- Iteration 9 --
bool(true)
bool(true)
bool(true)
string(3) "0.5"
string(3) "0.5"
string(3) "0.5"

-- Iteration 10 --
bool(true)
bool(true)
bool(true)
string(0) ""
string(0) ""
string(0) ""

-- Iteration 11 --
bool(true)
bool(true)
bool(true)
string(0) ""
string(0) ""
string(0) ""

-- Iteration 12 --
bool(true)
bool(true)
bool(true)
string(1) "1"
string(1) "1"
string(1) "1"

-- Iteration 13 --
bool(true)
bool(true)
bool(true)
string(0) ""
string(0) ""
string(0) ""

-- Iteration 14 --
bool(true)
bool(true)
bool(true)
string(1) "1"
string(1) "1"
string(1) "1"

-- Iteration 15 --
bool(true)
bool(true)
bool(true)
string(0) ""
string(0) ""
string(0) ""

-- Iteration 16 --
bool(true)
bool(true)
bool(true)
string(0) ""
string(0) ""
string(0) ""

-- Iteration 17 --
bool(true)
bool(true)
bool(true)
string(0) ""
string(0) ""
string(0) ""

-- Iteration 18 --
bool(true)
bool(true)
bool(true)
string(7) "Nothing"
string(7) "Nothing"
string(7) "Nothing"

-- Iteration 19 --
bool(true)
bool(true)
bool(true)
string(7) "Nothing"
string(7) "Nothing"
string(7) "Nothing"

-- Iteration 20 --
bool(true)
bool(true)
bool(true)
string(7) "Nothing"
string(7) "Nothing"
string(7) "Nothing"

-- Iteration 21 --
bool(true)
bool(true)
bool(true)
string(5) "UTF-8"
string(5) "UTF-8"
string(5) "UTF-8"

-- Iteration 22 --
bool(true)
bool(true)
bool(true)
string(0) ""
string(0) ""
string(0) ""

-- Iteration 23 --
bool(true)
bool(true)
bool(true)
string(0) ""
string(0) ""
string(0) ""

-- Iteration 24 --

Warning: iconv_set_encoding() expects parameter 2 to be string, resource given in %s on line %d
NULL

Warning: iconv_set_encoding() expects parameter 2 to be string, resource given in %s on line %d
NULL

Warning: iconv_set_encoding() expects parameter 2 to be string, resource given in %s on line %d
NULL
string(0) ""
string(0) ""
string(0) ""
Done