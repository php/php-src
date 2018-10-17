--TEST--
Test iconv_mime_decode() function : usage variations - Pass different data types to charset arg
--SKIPIF--
<?php
extension_loaded('iconv') or die('skip');
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
// Some of the parameters actually passed to charset will request to use
// a default charset determined by the platform. In order for this test to
// run on both linux and windows, the subject will have to be ascii only.
$header = 'Subject: =?UTF-8?B?QSBTYW1wbGUgVGVzdA==?=';
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
  $res = iconv_mime_decode($header, $mode, $input);
  if ($res !== false) {
  	 var_dump(bin2hex($res));
  }
  else {
     var_dump($res);
  }
  $iterator++;
};

fclose($fp);

echo "Done";
?>
--EXPECTF--
*** Testing iconv_mime_decode() : usage variations ***

-- Iteration 1 --

Notice: iconv_mime_decode(): Wrong charset, conversion from `???' to `0' is not allowed in %s on line %d
bool(false)

-- Iteration 2 --

Notice: iconv_mime_decode(): Wrong charset, conversion from `???' to `1' is not allowed in %s on line %d
bool(false)

-- Iteration 3 --

Notice: iconv_mime_decode(): Wrong charset, conversion from `???' to `12345' is not allowed in %s on line %d
bool(false)

-- Iteration 4 --

Notice: iconv_mime_decode(): Wrong charset, conversion from `???' to `-2345' is not allowed in %s on line %d
bool(false)

-- Iteration 5 --

Notice: iconv_mime_decode(): Wrong charset, conversion from `???' to `10.5' is not allowed in %s on line %d
bool(false)

-- Iteration 6 --

Notice: iconv_mime_decode(): Wrong charset, conversion from `???' to `-10.5' is not allowed in %s on line %d
bool(false)

-- Iteration 7 --

Notice: iconv_mime_decode(): Wrong charset, conversion from `???' to `123456789000' is not allowed in %s on line %d
bool(false)

-- Iteration 8 --

Notice: iconv_mime_decode(): Wrong charset, conversion from `???' to `1.23456789E-9' is not allowed in %s on line %d
bool(false)

-- Iteration 9 --

Notice: iconv_mime_decode(): Wrong charset, conversion from `???' to `0.5' is not allowed in %s on line %d
bool(false)

-- Iteration 10 --
string(44) "5375626a6563743a20412053616d706c652054657374"

-- Iteration 11 --
string(44) "5375626a6563743a20412053616d706c652054657374"

-- Iteration 12 --

Notice: iconv_mime_decode(): Wrong charset, conversion from `???' to `1' is not allowed in %s on line %d
bool(false)

-- Iteration 13 --
string(44) "5375626a6563743a20412053616d706c652054657374"

-- Iteration 14 --

Notice: iconv_mime_decode(): Wrong charset, conversion from `???' to `1' is not allowed in %s on line %d
bool(false)

-- Iteration 15 --
string(44) "5375626a6563743a20412053616d706c652054657374"

-- Iteration 16 --
string(44) "5375626a6563743a20412053616d706c652054657374"

-- Iteration 17 --
string(44) "5375626a6563743a20412053616d706c652054657374"

-- Iteration 18 --

Notice: iconv_mime_decode(): Wrong charset, conversion from `???' to `string' is not allowed in %s on line %d
bool(false)

-- Iteration 19 --

Notice: iconv_mime_decode(): Wrong charset, conversion from `???' to `string' is not allowed in %s on line %d
bool(false)

-- Iteration 20 --

Notice: iconv_mime_decode(): Wrong charset, conversion from `???' to `hello world' is not allowed in %s on line %d
bool(false)

-- Iteration 21 --

Notice: iconv_mime_decode(): Wrong charset, conversion from `???' to `Class A object' is not allowed in %s on line %d
bool(false)

-- Iteration 22 --
string(44) "5375626a6563743a20412053616d706c652054657374"

-- Iteration 23 --
string(44) "5375626a6563743a20412053616d706c652054657374"

-- Iteration 24 --

Warning: iconv_mime_decode() expects parameter 3 to be string, resource given in %s on line %d
bool(false)
Done
