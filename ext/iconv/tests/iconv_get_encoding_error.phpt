--TEST--
Test iconv_get_encoding() function : basic functionality
--SKIPIF--
<?php
extension_loaded('iconv') or die('skip');
function_exists('iconv_get_encoding') or die("skip iconv_get_encoding() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : mixed iconv_get_encoding([string type])
 * Description: Get internal encoding and output encoding for ob_iconv_handler() 
 * Source code: ext/iconv/iconv.c 
 */

/*
 * Test Error functionality of iconv_get_encoding
 */

echo "*** Testing iconv_get_encoding() : error functionality ***\n";

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
  var_dump( iconv_get_encoding($input) );
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing iconv_get_encoding() : error functionality ***

-- Iteration 1 --
bool(false)

-- Iteration 2 --
bool(false)

-- Iteration 3 --
bool(false)

-- Iteration 4 --
bool(false)

-- Iteration 5 --
bool(false)

-- Iteration 6 --
bool(false)

-- Iteration 7 --
bool(false)

-- Iteration 8 --
bool(false)

-- Iteration 9 --
bool(false)

-- Iteration 10 --
bool(false)

-- Iteration 11 --
bool(false)

-- Iteration 12 --
bool(false)

-- Iteration 13 --
bool(false)

-- Iteration 14 --
bool(false)

-- Iteration 15 --
bool(false)

-- Iteration 16 --
bool(false)

-- Iteration 17 --
bool(false)

-- Iteration 18 --
bool(false)

-- Iteration 19 --
bool(false)

-- Iteration 20 --
bool(false)

-- Iteration 21 --
bool(false)

-- Iteration 22 --
bool(false)

-- Iteration 23 --
bool(false)

-- Iteration 24 --

Warning: iconv_get_encoding() expects parameter 1 to be string, resource given in %s on line %d
NULL
Done