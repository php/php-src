--TEST--
Test iconv_strlen() function : usage variations - Pass different data types as $str arg
--SKIPIF--
<?php
extension_loaded('iconv') or die('skip');
function_exists('iconv_strlen') or die("skip iconv_strlen() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : int iconv_strlen(string str [, string charset])
 * Description: Get character numbers of a string
 * Source code: ext/iconv/iconv.c
 */

/*
 * Test iconv_strlen by passing different data types as $str argument
 */

echo "*** Testing iconv_strlen() : usage variations ***\n";

// Initialise function arguments not being substituted
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

// unexpected values to be passed to $str argument
$inputs = array(

       // int data
/*1*/
	 'int 0' => 0,
     'int 1' => 1,
     'int 12345' => 12345,
     'int -12345' => -12345,

       // float data
/*5*/
	 'float 10.5' => 10.5,
     'float -10.5' => -10.5,
     'float 12.3456789000e10' => 12.3456789000e10,
     'float 12.3456789000e-10' => 12.3456789000e-10,
     'float .5' => .5,

       // null data
/*10*/
      'uppercase NULL' => NULL,
      'lowercase null' => null,

       // boolean data
/*12*/
      'lowercase true' => true,
      'lowercase false' =>false,
      'uppercase TRUE' =>TRUE,
      'uppercase FALSE' =>FALSE,

       // empty data
/*16*/
      'empty string DQ' => "",
      'empty string SQ' => '',

       // string data
/*18*/
      'string DQ' => "string",
      'string SQ' => 'string',
      'mixed case string' => "sTrInG",
      'heredoc' => $heredoc,

       // object data
/*21*/
      'instance of class' => new classA(),

       // undefined data
/*22*/
      'undefined var' => @$undefined_var,

       // unset data
/*23*/
      'unset var' => @$unset_var,

       // resource variable
/*24*/
       'resource' => $fp
);

// loop through each element of $inputs to check the behavior of iconv_strlen()
$iterator = 1;
foreach($inputs as $key =>$value) {
  echo "\n--$key--\n";
  var_dump( iconv_strlen($value, $encoding));
  $iterator++;
};

fclose($fp);
?>
===DONE===
--EXPECTF--
*** Testing iconv_strlen() : usage variations ***

--int 0--
int(1)

--int 1--
int(1)

--int 12345--
int(5)

--int -12345--
int(6)

--float 10.5--
int(4)

--float -10.5--
int(5)

--float 12.3456789000e10--
int(12)

--float 12.3456789000e-10--
int(13)

--float .5--
int(3)

--uppercase NULL--
int(0)

--lowercase null--
int(0)

--lowercase true--
int(1)

--lowercase false--
int(0)

--uppercase TRUE--
int(1)

--uppercase FALSE--
int(0)

--empty string DQ--
int(0)

--empty string SQ--
int(0)

--string DQ--
int(6)

--string SQ--
int(6)

--mixed case string--
int(6)

--heredoc--
int(11)

--instance of class--
int(14)

--undefined var--
int(0)

--unset var--
int(0)

--resource--

Warning: iconv_strlen() expects parameter 1 to be string, resource given in %s on line %d
bool(false)
===DONE===
