--TEST--
Test mb_encode_mimeheader() function : usage variations - Pass different data types to $transfer_encoding arg
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_encode_mimeheader') or die("skip mb_encode_mimeheader() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_encode_mimeheader
 * (string $str [, string $charset [, string $transfer_encoding [, string $linefeed [, int $indent]]]])
 * Description: Converts the string to MIME "encoded-word" in the format of =?charset?(B|Q)?encoded_string?= 
 * Source code: ext/mbstring/mbstring.c
 */

/*
 * Pass different data types to $transfer_encoding argument to see how mb_encode_mimeheader() behaves
 */

echo "*** Testing mb_encode_mimeheader() : usage variations ***\n";

mb_internal_encoding('utf-8');

// Initialise function arguments not being substituted
$str = base64_decode('5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC');
$charset = 'utf-8';
$linefeed = "\r\n";
$indent = 2;

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

// unexpected values to be passed to $transfer_encoding argument
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

// loop through each element of $inputs to check the behavior of mb_encode_mimeheader()
$iterator = 1;
foreach($inputs as $input) {
  echo "\n-- Iteration $iterator --\n";
  var_dump( mb_encode_mimeheader($str, $charset, $input, $linefeed, $indent));
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing mb_encode_mimeheader() : usage variations ***

-- Iteration 1 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 2 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 3 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 4 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 5 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 6 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 7 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 8 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 9 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 10 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 11 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 12 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 13 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 14 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 15 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 16 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 17 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 18 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 19 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 20 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 21 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 22 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 23 --
string(52) "=?UTF-8?B?5pel5pys6Kqe44OG44Kt44K544OI44Gn44GZ44CC?="

-- Iteration 24 --

Warning: mb_encode_mimeheader() expects parameter 3 to be string, resource given in %s on line %d
NULL
Done
