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
      var_dump(bin2hex(mcrypt_ecb($cipher, $value, $data, $mode, $iv)));
};

fclose($fp);

?>
===DONE===
--EXPECTF--
*** Testing mcrypt_ecb() : usage variation ***

--int 0--
string(32) "e469e6b066f9600e1eefd8f53365f96c"

--int 1--
string(32) "e469e6b066f9600e1eefd8f53365f96c"

--int 12345--
string(32) "d74e5f51d1199bcfa61f80168e913007"

--int -12345--
string(32) "17fe485ed735abb34c1dd4455af7b79c"

--float 10.5--
string(32) "cd735509aa4013a130e011686d66ae01"

--float -10.5--
string(32) "a57d99d6d5813039abf50fc50d631e47"

--float 12.3456789000e10--
string(32) "f17ede0bfdaa4408f545f7f4c8b040d2"

--float -12.3456789000e10--
string(32) "326f64e3b9bd5a6beb0a9b52a09a5a48"

--float .5--
string(32) "2aedf7661cd4d8c7593f44c58718e2b8"

--empty array--
Error: 2 - mcrypt_ecb() expects parameter 2 to be string, array given, %s(%d)
string(0) ""

--int indexed array--
Error: 2 - mcrypt_ecb() expects parameter 2 to be string, array given, %s(%d)
string(0) ""

--associative array--
Error: 2 - mcrypt_ecb() expects parameter 2 to be string, array given, %s(%d)
string(0) ""

--nested arrays--
Error: 2 - mcrypt_ecb() expects parameter 2 to be string, array given, %s(%d)
string(0) ""

--uppercase NULL--
string(32) "bfa2cb7240c8d2f6abeb34960c04f6d3"

--lowercase null--
string(32) "bfa2cb7240c8d2f6abeb34960c04f6d3"

--lowercase true--
string(32) "e469e6b066f9600e1eefd8f53365f96c"

--lowercase false--
string(32) "bfa2cb7240c8d2f6abeb34960c04f6d3"

--uppercase TRUE--
string(32) "e469e6b066f9600e1eefd8f53365f96c"

--uppercase FALSE--
string(32) "bfa2cb7240c8d2f6abeb34960c04f6d3"

--empty string DQ--
string(32) "bfa2cb7240c8d2f6abeb34960c04f6d3"

--empty string SQ--
string(32) "bfa2cb7240c8d2f6abeb34960c04f6d3"

--instance of classWithToString--
string(32) "1fd3514d8ced44d04d9dc7511fce33ef"

--instance of classWithoutToString--
Error: 2 - mcrypt_ecb() expects parameter 2 to be string, object given, %s(%d)
string(0) ""

--undefined var--
string(32) "bfa2cb7240c8d2f6abeb34960c04f6d3"

--unset var--
string(32) "bfa2cb7240c8d2f6abeb34960c04f6d3"

--resource--
Error: 2 - mcrypt_ecb() expects parameter 2 to be string, resource given, %s(%d)
string(0) ""
===DONE===

