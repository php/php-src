--TEST--
Test mb_decode_mimeheader() function : usage variation
--CREDITS--
D. Kesley
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_decode_mimeheader') or die("skip mb_decode_mimeheader() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_decode_mimeheader(string string)
 * Description: Decodes the MIME "encoded-word" in the string
 * Source code: ext/mbstring/mbstring.c
 * Alias to functions:
 */

echo "*** Testing mb_decode_mimeheader() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// define some classes
class classWithToString
{
	public function __toString() {
		return "Class A object";
	}
}

class classWithoutToString
{
}

// heredoc string
$heredoc = <<<EOT
hello world
EOT;

// get a resource variable
$fp = fopen(__FILE__, "r");

// add arrays
$index_array = array (1, 2, 3);
$assoc_array = array ('one' => 1, 'two' => 2);

//array of values to iterate over
$inputs = array(

      // int data
      'int 0' => 0,
      'int 1' => 1,
      'int 12345' => 12345,
      'int -12345' => -2345,

      // float data
      'float 10.5' => 10.5,
      'float -10.5' => -10.5,
      'float 12.3456789000e10' => 12.3456789000e10,
      'float -12.3456789000e10' => -12.3456789000e10,
      'float .5' => .5,

      // array data
      'empty array' => array(),
      'int indexed array' => $index_array,
      'associative array' => $assoc_array,
      'nested arrays' => array('foo', $index_array, $assoc_array),

      // null data
      'uppercase NULL' => NULL,
      'lowercase null' => null,

      // boolean data
      'lowercase true' => true,
      'lowercase false' =>false,
      'uppercase TRUE' =>TRUE,
      'uppercase FALSE' =>FALSE,

      // empty data
      'empty string DQ' => "",
      'empty string SQ' => '',

      // object data
      'instance of classWithToString' => new classWithToString(),
      'instance of classWithoutToString' => new classWithoutToString(),

      // undefined data
      'undefined var' => @$undefined_var,

      // unset data
      'unset var' => @$unset_var,

      // resource variable
      'resource' => $fp
);

// loop through each element of the array for string

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( mb_decode_mimeheader($value) );
};

fclose($fp);

?>
===DONE===
--EXPECTF--
*** Testing mb_decode_mimeheader() : usage variation ***

--int 0--
string(1) "0"

--int 1--
string(1) "1"

--int 12345--
string(5) "12345"

--int -12345--
string(5) "-2345"

--float 10.5--
string(4) "10.5"

--float -10.5--
string(5) "-10.5"

--float 12.3456789000e10--
string(12) "123456789000"

--float -12.3456789000e10--
string(13) "-123456789000"

--float .5--
string(3) "0.5"

--empty array--
Error: 2 - mb_decode_mimeheader() expects parameter 1 to be string, array given, %s(%d)
NULL

--int indexed array--
Error: 2 - mb_decode_mimeheader() expects parameter 1 to be string, array given, %s(%d)
NULL

--associative array--
Error: 2 - mb_decode_mimeheader() expects parameter 1 to be string, array given, %s(%d)
NULL

--nested arrays--
Error: 2 - mb_decode_mimeheader() expects parameter 1 to be string, array given, %s(%d)
NULL

--uppercase NULL--
string(0) ""

--lowercase null--
string(0) ""

--lowercase true--
string(1) "1"

--lowercase false--
string(0) ""

--uppercase TRUE--
string(1) "1"

--uppercase FALSE--
string(0) ""

--empty string DQ--
string(0) ""

--empty string SQ--
string(0) ""

--instance of classWithToString--
string(14) "Class A object"

--instance of classWithoutToString--
Error: 2 - mb_decode_mimeheader() expects parameter 1 to be string, object given, %s(%d)
NULL

--undefined var--
string(0) ""

--unset var--
string(0) ""

--resource--
Error: 2 - mb_decode_mimeheader() expects parameter 1 to be string, resource given, %s(%d)
NULL
===DONE===
