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
$key = b'string_val';
$data = b'string_val';
$mode = MCRYPT_ENCRYPT;

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
      var_dump(bin2hex( mcrypt_ecb($cipher, $key, $data, $mode, $value)));
};

fclose($fp);

?>
===DONE===
--EXPECTF--
*** Testing mcrypt_ecb() : usage variation ***

--int 0--
string(32) "6438db90653c4d300909aa02fd6163c2"

--int 1--
string(32) "6438db90653c4d300909aa02fd6163c2"

--int 12345--
string(32) "6438db90653c4d300909aa02fd6163c2"

--int -12345--
string(32) "6438db90653c4d300909aa02fd6163c2"

--float 10.5--
string(32) "6438db90653c4d300909aa02fd6163c2"

--float -10.5--
string(32) "6438db90653c4d300909aa02fd6163c2"

--float 12.3456789000e10--
string(32) "6438db90653c4d300909aa02fd6163c2"

--float -12.3456789000e10--
string(32) "6438db90653c4d300909aa02fd6163c2"

--float .5--
string(32) "6438db90653c4d300909aa02fd6163c2"

--empty array--
Error: 2 - mcrypt_ecb() expects parameter 5 to be string, array given, %s(%d)
string(0) ""

--int indexed array--
Error: 2 - mcrypt_ecb() expects parameter 5 to be string, array given, %s(%d)
string(0) ""

--associative array--
Error: 2 - mcrypt_ecb() expects parameter 5 to be string, array given, %s(%d)
string(0) ""

--nested arrays--
Error: 2 - mcrypt_ecb() expects parameter 5 to be string, array given, %s(%d)
string(0) ""

--uppercase NULL--
string(32) "6438db90653c4d300909aa02fd6163c2"

--lowercase null--
string(32) "6438db90653c4d300909aa02fd6163c2"

--lowercase true--
string(32) "6438db90653c4d300909aa02fd6163c2"

--lowercase false--
string(32) "6438db90653c4d300909aa02fd6163c2"

--uppercase TRUE--
string(32) "6438db90653c4d300909aa02fd6163c2"

--uppercase FALSE--
string(32) "6438db90653c4d300909aa02fd6163c2"

--empty string DQ--
string(32) "6438db90653c4d300909aa02fd6163c2"

--empty string SQ--
string(32) "6438db90653c4d300909aa02fd6163c2"

--instance of classWithToString--
string(32) "6438db90653c4d300909aa02fd6163c2"

--instance of classWithoutToString--
Error: 2 - mcrypt_ecb() expects parameter 5 to be string, object given, %s(%d)
string(0) ""

--undefined var--
string(32) "6438db90653c4d300909aa02fd6163c2"

--unset var--
string(32) "6438db90653c4d300909aa02fd6163c2"

--resource--
Error: 2 - mcrypt_ecb() expects parameter 5 to be string, resource given, %s(%d)
string(0) ""
===DONE===

