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
$key = b'string_val';
$data = b'string_val';
$mode = MCRYPT_ENCRYPT;
$iv = b'string_val';

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

// loop through each element of the array for cipher

foreach($inputs as $valueType =>$value) {
      echo "\n--$valueType--\n";
      var_dump( mcrypt_ecb($value, $key, $data, $mode, $iv) );
};

fclose($fp);

?>
===DONE===
--EXPECTF--
*** Testing mcrypt_ecb() : usage variation ***

--int 0--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--int 1--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--int 12345--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--int -12345--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--float 10.5--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--float -10.5--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--float 12.3456789000e10--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--float -12.3456789000e10--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--float .5--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--empty array--
Error: 2 - mcrypt_ecb() expects parameter 1 to be string, array given, %s(%d)
NULL

--int indexed array--
Error: 2 - mcrypt_ecb() expects parameter 1 to be string, array given, %s(%d)
NULL

--associative array--
Error: 2 - mcrypt_ecb() expects parameter 1 to be string, array given, %s(%d)
NULL

--nested arrays--
Error: 2 - mcrypt_ecb() expects parameter 1 to be string, array given, %s(%d)
NULL

--uppercase NULL--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--lowercase null--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--lowercase true--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--lowercase false--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--uppercase TRUE--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--uppercase FALSE--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--empty string DQ--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--empty string SQ--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--instance of classWithToString--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--instance of classWithoutToString--
Error: 2 - mcrypt_ecb() expects parameter 1 to be string, object given, %s(%d)
NULL

--undefined var--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--unset var--
Error: 2 - mcrypt_ecb(): Module initialization failed, %s(%d)
bool(false)

--resource--
Error: 2 - mcrypt_ecb() expects parameter 1 to be string, resource given, %s(%d)
NULL
===DONE===

