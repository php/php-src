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
/* Prototype  : string mcrypt_ecb(string cipher, string key, string data, int mode, string iv)
 * Description: ECB crypt/decrypt data using key key with cipher cipher starting with iv 
 * Source code: ext/mcrypt/mcrypt.c
 * Alias to functions: 
 */

echo "*** Testing mcrypt_ecb() : usage variation ***\n";

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
$mode = MCRYPT_ENCRYPT;

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
unicode(32) "6438db90653c4d300909aa02fd6163c2"

--int 1--
unicode(32) "6438db90653c4d300909aa02fd6163c2"

--int 12345--
unicode(32) "6438db90653c4d300909aa02fd6163c2"

--int -12345--
unicode(32) "6438db90653c4d300909aa02fd6163c2"

--float 10.5--
unicode(32) "6438db90653c4d300909aa02fd6163c2"

--float -10.5--
unicode(32) "6438db90653c4d300909aa02fd6163c2"

--float 12.3456789000e10--
unicode(32) "6438db90653c4d300909aa02fd6163c2"

--float -12.3456789000e10--
unicode(32) "6438db90653c4d300909aa02fd6163c2"

--float .5--
unicode(32) "6438db90653c4d300909aa02fd6163c2"

--empty array--
Error: 2 - mcrypt_ecb() expects parameter 5 to be binary string, array given, %s(%d)
unicode(0) ""

--int indexed array--
Error: 2 - mcrypt_ecb() expects parameter 5 to be binary string, array given, %s(%d)
unicode(0) ""

--associative array--
Error: 2 - mcrypt_ecb() expects parameter 5 to be binary string, array given, %s(%d)
unicode(0) ""

--nested arrays--
Error: 2 - mcrypt_ecb() expects parameter 5 to be binary string, array given, %s(%d)
unicode(0) ""

--uppercase NULL--
unicode(32) "6438db90653c4d300909aa02fd6163c2"

--lowercase null--
unicode(32) "6438db90653c4d300909aa02fd6163c2"

--lowercase true--
unicode(32) "6438db90653c4d300909aa02fd6163c2"

--lowercase false--
unicode(32) "6438db90653c4d300909aa02fd6163c2"

--uppercase TRUE--
unicode(32) "6438db90653c4d300909aa02fd6163c2"

--uppercase FALSE--
unicode(32) "6438db90653c4d300909aa02fd6163c2"

--empty string DQ--
Error: 2 - mcrypt_ecb() expects parameter 5 to be strictly a binary string, Unicode string given, %s(%d)
unicode(0) ""

--empty string SQ--
Error: 2 - mcrypt_ecb() expects parameter 5 to be strictly a binary string, Unicode string given, %s(%d)
unicode(0) ""

--instance of classWithToString--
unicode(32) "6438db90653c4d300909aa02fd6163c2"

--instance of classWithoutToString--
Error: 2 - mcrypt_ecb() expects parameter 5 to be binary string, object given, %s(%d)
unicode(0) ""

--undefined var--
unicode(32) "6438db90653c4d300909aa02fd6163c2"

--unset var--
unicode(32) "6438db90653c4d300909aa02fd6163c2"

--resource--
Error: 2 - mcrypt_ecb() expects parameter 5 to be binary string, resource given, %s(%d)
unicode(0) ""
===DONE===

