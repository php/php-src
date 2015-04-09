--TEST--
Test mcrypt_ecb() function : usage variation 
--SKIPIF--
<?php 
if (!extension_loaded("mcrypt")) {
	print "skip - mcrypt extension not loaded"; 
}	 
?>
--FILE--
<?php
error_reporting(E_ALL & ~E_DEPRECATED);

/* Prototype  : string mcrypt_ecb(string cipher, string key, string data, int mode, string iv)
 * Description: ECB crypt/decrypt data using key key with cipher cipher starting with iv 
 * Source code: ext/mcrypt/mcrypt.c
 * Alias to functions: 
 */

echo "*** Testing mcrypt_ecb() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if ($err_no & error_reporting()) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)
$cipher = MCRYPT_TRIPLEDES;
$key = b"string_val\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
$mode = MCRYPT_ENCRYPT;
$iv = b'01234567';

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

// loop through each element of the array for data

foreach($inputs as $valueType =>$value) {
      echo "\n--$valueType--\n";
      var_dump(bin2hex(mcrypt_ecb($cipher, $key, $value, $mode, $iv)));
};

fclose($fp);

?>
===DONE===
--EXPECTF--
*** Testing mcrypt_ecb() : usage variation ***

--int 0--
string(16) "51dc9cd9179b718b"

--int 1--
string(16) "619c335f8c4f9cbf"

--int 12345--
string(16) "b1258d67ab73de00"

--int -12345--
string(16) "8eecf134443bd6b9"

--float 10.5--
string(16) "34b5750a793baff5"

--float -10.5--
string(16) "7a605f2aacc8a11d"

--float 12.3456789000e10--
string(32) "74a0d7026ae586f476d4b17808851e86"

--float -12.3456789000e10--
string(32) "bfb155997017986c01090afebd62c7ca"

--float .5--
string(16) "cc60ac201164b6c7"

--empty array--
Error: 2 - mcrypt_ecb() expects parameter 3 to be string, array given, %s(%d)
string(0) ""

--int indexed array--
Error: 2 - mcrypt_ecb() expects parameter 3 to be string, array given, %s(%d)
string(0) ""

--associative array--
Error: 2 - mcrypt_ecb() expects parameter 3 to be string, array given, %s(%d)
string(0) ""

--nested arrays--
Error: 2 - mcrypt_ecb() expects parameter 3 to be string, array given, %s(%d)
string(0) ""

--uppercase NULL--
string(16) "6ece228c41457539"

--lowercase null--
string(16) "6ece228c41457539"

--lowercase true--
string(16) "619c335f8c4f9cbf"

--lowercase false--
string(16) "6ece228c41457539"

--uppercase TRUE--
string(16) "619c335f8c4f9cbf"

--uppercase FALSE--
string(16) "6ece228c41457539"

--empty string DQ--
string(16) "6ece228c41457539"

--empty string SQ--
string(16) "6ece228c41457539"

--instance of classWithToString--
string(32) "749c3b4d16731d98370128754b7c930f"

--instance of classWithoutToString--
Error: 2 - mcrypt_ecb() expects parameter 3 to be string, object given, %s(%d)
string(0) ""

--undefined var--
string(16) "6ece228c41457539"

--unset var--
string(16) "6ece228c41457539"

--resource--
Error: 2 - mcrypt_ecb() expects parameter 3 to be string, resource given, %s(%d)
string(0) ""
===DONE===

