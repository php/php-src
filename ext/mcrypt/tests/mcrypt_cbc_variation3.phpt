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
$key = b'string_val';
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
      var_dump(bin2hex(mcrypt_cbc($cipher, $key, $value, $mode, $iv)));
};

fclose($fp);

?>
===DONE===
--EXPECTF--
*** Testing mcrypt_cbc() : usage variation ***

--int 0--
string(16) "ce5fcfe737859795"

--int 1--
string(16) "84df495f6cd82dd9"

--int 12345--
string(16) "905ab1ae27ee9991"

--int -12345--
string(16) "5835174e9c67c3e7"

--float 10.5--
string(16) "28ff0601ad9e47fa"

--float -10.5--
string(16) "ce9f2b6e2fc3d9f7"

--float 12.3456789000e10--
string(32) "24eb882ce9763e4018fba9b7f01b0c3e"

--float -12.3456789000e10--
string(32) "5eed30e428f32de1d7a7064d0ed4d3eb"

--float .5--
string(16) "bebf2a13676e1e30"

--empty array--
Error: 2 - mcrypt_cbc() expects parameter 3 to be string, array given, %s(%d)
string(0) ""

--int indexed array--
Error: 2 - mcrypt_cbc() expects parameter 3 to be string, array given, %s(%d)
string(0) ""

--associative array--
Error: 2 - mcrypt_cbc() expects parameter 3 to be string, array given, %s(%d)
string(0) ""

--nested arrays--
Error: 2 - mcrypt_cbc() expects parameter 3 to be string, array given, %s(%d)
string(0) ""

--uppercase NULL--
string(16) "206f6d3617a5ab32"

--lowercase null--
string(16) "206f6d3617a5ab32"

--lowercase true--
string(16) "84df495f6cd82dd9"

--lowercase false--
string(16) "206f6d3617a5ab32"

--uppercase TRUE--
string(16) "84df495f6cd82dd9"

--uppercase FALSE--
string(16) "206f6d3617a5ab32"

--empty string DQ--
string(16) "206f6d3617a5ab32"

--empty string SQ--
string(16) "206f6d3617a5ab32"

--instance of classWithToString--
string(32) "7c91cdf8f8c51485034a9ee528eb016b"

--instance of classWithoutToString--
Error: 2 - mcrypt_cbc() expects parameter 3 to be string, object given, %s(%d)
string(0) ""

--undefined var--
string(16) "206f6d3617a5ab32"

--unset var--
string(16) "206f6d3617a5ab32"

--resource--
Error: 2 - mcrypt_cbc() expects parameter 3 to be string, resource given, %s(%d)
string(0) ""
===DONE===

