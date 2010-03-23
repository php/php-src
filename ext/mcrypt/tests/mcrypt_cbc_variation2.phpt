--TEST--
Test mcrypt_cbc() function : usage variation 
--SKIPIF--
<?php 
if (!extension_loaded("mcrypt")) {
	print "skip - mcrypt extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : string mcrypt_cbc(string cipher, string key, string data, int mode, string iv)
 * Description: CBC crypt/decrypt data using key key with cipher cipher starting with iv 
 * Source code: ext/mcrypt/mcrypt.c
 * Alias to functions: 
 */

echo "*** Testing mcrypt_cbc() : usage variation ***\n";

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
$data = b'string_val';
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

// loop through each element of the array for key

foreach($inputs as $valueType =>$value) {
      echo "\n--$valueType--\n";
      var_dump(bin2hex(mcrypt_cbc($cipher, $value, $data, $mode, $iv)));
};

fclose($fp);

?>
===DONE===
--EXPECTF--
*** Testing mcrypt_cbc() : usage variation ***

--int 0--
string(32) "bc27b3a4e33b531d5983fc7df693cd09"

--int 1--
string(32) "bc27b3a4e33b531d5983fc7df693cd09"

--int 12345--
string(32) "d109b7973383127002474ae731c4b3a8"

--int -12345--
string(32) "3e82a931cedb03a38b91a637ff8c9f9e"

--float 10.5--
string(32) "de71833586c1d7132a289960ebeeca7a"

--float -10.5--
string(32) "7d0489dd2e99ae910ecc015573f3dd16"

--float 12.3456789000e10--
string(32) "978055b42c0506a8947e3c3c8d994baf"

--float -12.3456789000e10--
string(32) "4aa84ba400c2b8ef467d4d98372b4f4e"

--float .5--
string(32) "e731dc5059b84e0c8774ac490f77d6e6"

--empty array--
Error: 2 - mcrypt_cbc() expects parameter 2 to be string, array given, %s(%d)
string(0) ""

--int indexed array--
Error: 2 - mcrypt_cbc() expects parameter 2 to be string, array given, %s(%d)
string(0) ""

--associative array--
Error: 2 - mcrypt_cbc() expects parameter 2 to be string, array given, %s(%d)
string(0) ""

--nested arrays--
Error: 2 - mcrypt_cbc() expects parameter 2 to be string, array given, %s(%d)
string(0) ""

--uppercase NULL--
string(32) "be722a5ffc361d721fbcab1eacc6acf5"

--lowercase null--
string(32) "be722a5ffc361d721fbcab1eacc6acf5"

--lowercase true--
string(32) "bc27b3a4e33b531d5983fc7df693cd09"

--lowercase false--
string(32) "be722a5ffc361d721fbcab1eacc6acf5"

--uppercase TRUE--
string(32) "bc27b3a4e33b531d5983fc7df693cd09"

--uppercase FALSE--
string(32) "be722a5ffc361d721fbcab1eacc6acf5"

--empty string DQ--
string(32) "be722a5ffc361d721fbcab1eacc6acf5"

--empty string SQ--
string(32) "be722a5ffc361d721fbcab1eacc6acf5"

--instance of classWithToString--
string(32) "19420fa26f561ee82ed84abbcd2d284b"

--instance of classWithoutToString--
Error: 2 - mcrypt_cbc() expects parameter 2 to be string, object given, %s(%d)
string(0) ""

--undefined var--
string(32) "be722a5ffc361d721fbcab1eacc6acf5"

--unset var--
string(32) "be722a5ffc361d721fbcab1eacc6acf5"

--resource--
Error: 2 - mcrypt_cbc() expects parameter 2 to be string, resource given, %s(%d)
string(0) ""
===DONE===

