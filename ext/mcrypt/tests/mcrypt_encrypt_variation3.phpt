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
$mode = MCRYPT_MODE_ECB;
$iv = b'01234567';

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
      var_dump( bin2hex(mcrypt_encrypt($cipher, $key, $value, $mode, $iv) ));
};

fclose($fp);

?>
===DONE===
--EXPECTF--
*** Testing mcrypt_encrypt() : usage variation ***

--int 0--
unicode(16) "51dc9cd9179b718b"

--int 1--
unicode(16) "619c335f8c4f9cbf"

--int 12345--
unicode(16) "b1258d67ab73de00"

--int -12345--
unicode(16) "8eecf134443bd6b9"

--float 10.5--
unicode(16) "34b5750a793baff5"

--float -10.5--
unicode(16) "7a605f2aacc8a11d"

--float 12.3456789000e10--
unicode(32) "74a0d7026ae586f476d4b17808851e86"

--float -12.3456789000e10--
unicode(32) "bfb155997017986c01090afebd62c7ca"

--float .5--
unicode(16) "cc60ac201164b6c7"

--empty array--
Error: 2 - mcrypt_encrypt() expects parameter 3 to be binary string, array given, %s(%d)
unicode(0) ""

--int indexed array--
Error: 2 - mcrypt_encrypt() expects parameter 3 to be binary string, array given, %s(%d)
unicode(0) ""

--associative array--
Error: 2 - mcrypt_encrypt() expects parameter 3 to be binary string, array given, %s(%d)
unicode(0) ""

--nested arrays--
Error: 2 - mcrypt_encrypt() expects parameter 3 to be binary string, array given, %s(%d)
unicode(0) ""

--uppercase NULL--
unicode(16) "6ece228c41457539"

--lowercase null--
unicode(16) "6ece228c41457539"

--lowercase true--
unicode(16) "619c335f8c4f9cbf"

--lowercase false--
unicode(16) "6ece228c41457539"

--uppercase TRUE--
unicode(16) "619c335f8c4f9cbf"

--uppercase FALSE--
unicode(16) "6ece228c41457539"

--empty string DQ--
Error: 2 - mcrypt_encrypt() expects parameter 3 to be strictly a binary string, Unicode string given, %s(%d)
unicode(0) ""

--empty string SQ--
Error: 2 - mcrypt_encrypt() expects parameter 3 to be strictly a binary string, Unicode string given, %s(%d)
unicode(0) ""

--instance of classWithToString--
unicode(32) "749c3b4d16731d98370128754b7c930f"

--instance of classWithoutToString--
Error: 2 - mcrypt_encrypt() expects parameter 3 to be binary string, object given, %s(%d)
unicode(0) ""

--undefined var--
unicode(16) "6ece228c41457539"

--unset var--
unicode(16) "6ece228c41457539"

--resource--
Error: 2 - mcrypt_encrypt() expects parameter 3 to be binary string, resource given, %s(%d)
unicode(0) ""
===DONE===

