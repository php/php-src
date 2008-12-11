--TEST--
Test mcrypt_encrypt() function : usage variation 
--SKIPIF--
<?php 
if (!extension_loaded("mcrypt")) {
	print "skip - mcrypt extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : string mcrypt_encrypt(string cipher, string key, string data, string mode, string iv)
 * Description: OFB crypt/decrypt data using key key with cipher cipher starting with iv 
 * Source code: ext/mcrypt/mcrypt.c
 * Alias to functions: 
 */

echo "*** Testing mcrypt_encrypt() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)
$cipher = MCRYPT_TRIPLEDES;
$key = b'string_val';
$data = b'string_val';
//in php, it incorrectly reports problems with iv in ECB mode.
$mode = MCRYPT_MODE_CBC;

//get an unset variable
$unset_var = 10;
unset ($unset_var);

// define some classes
class classWithToString
{
	public function __toString() {
		return b"Class A object";
	}
}

class classWithoutToString
{
}

// heredoc string
$heredoc = b<<<EOT
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

// loop through each element of the array for iv

foreach($inputs as $valueType =>$value) {
      echo "\n--$valueType--\n";
      var_dump( bin2hex(mcrypt_encrypt($cipher, $key, $data, $mode, $value)));
};

fclose($fp);

?>
===DONE===
--EXPECTF--
*** Testing mcrypt_encrypt() : usage variation ***

--int 0--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--int 1--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--int 12345--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--int -12345--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--float 10.5--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--float -10.5--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--float 12.3456789000e10--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--float -12.3456789000e10--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--float .5--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--empty array--
Error: 2 - mcrypt_encrypt() expects parameter 5 to be string, array given, %s(%d)
string(0) ""

--int indexed array--
Error: 2 - mcrypt_encrypt() expects parameter 5 to be string, array given, %s(%d)
string(0) ""

--associative array--
Error: 2 - mcrypt_encrypt() expects parameter 5 to be string, array given, %s(%d)
string(0) ""

--nested arrays--
Error: 2 - mcrypt_encrypt() expects parameter 5 to be string, array given, %s(%d)
string(0) ""

--uppercase NULL--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--lowercase null--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--lowercase true--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--lowercase false--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--uppercase TRUE--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--uppercase FALSE--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--empty string DQ--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--empty string SQ--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--instance of classWithToString--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--instance of classWithoutToString--
Error: 2 - mcrypt_encrypt() expects parameter 5 to be string, object given, %s(%d)
string(0) ""

--undefined var--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--unset var--
Error: 2 - mcrypt_encrypt(): The IV parameter must be as long as the blocksize, %s(%d)
string(32) "6438db90653c4d3080c3ceab43618c05"

--resource--
Error: 2 - mcrypt_encrypt() expects parameter 5 to be string, resource given, %s(%d)
string(0) ""
===DONE===

