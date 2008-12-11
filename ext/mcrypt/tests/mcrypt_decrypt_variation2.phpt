--TEST--
Test mcrypt_decrypt() function : usage variation 
--SKIPIF--
<?php 
if (!extension_loaded("mcrypt")) {
	print "skip - mcrypt extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : string mcrypt_decrypt(string cipher, string key, string data, string mode, string iv)
 * Description: OFB crypt/decrypt data using key key with cipher cipher starting with iv 
 * Source code: ext/mcrypt/mcrypt.c
 * Alias to functions: 
 */

echo "*** Testing mcrypt_decrypt() : usage variation ***\n";

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
$mode = MCRYPT_MODE_ECB;
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
      var_dump( bin2hex(mcrypt_decrypt($cipher, $value, $data, $mode, $iv)));
};

fclose($fp);

?>
===DONE===
--EXPECTF--
*** Testing mcrypt_decrypt() : usage variation ***

--int 0--
string(32) "43a1ae011df36064589d06bc922ecd97"

--int 1--
string(32) "43a1ae011df36064589d06bc922ecd97"

--int 12345--
string(32) "e5885552e16c44d4eb6164f477b40200"

--int -12345--
string(32) "adf7873831a9035cda9f9dc3b7dc626b"

--float 10.5--
string(32) "08b0b9fac9c227437b7b5d0147e6153b"

--float -10.5--
string(32) "f470cc74d83471b42a3e28d4ec57799a"

--float 12.3456789000e10--
string(32) "36c618c00523fadc372b871eaa9c7b16"

--float -12.3456789000e10--
string(32) "a554a5bdb7a5ceb6ae6f20566ef02e49"

--float .5--
string(32) "bcb840ff76d3788a7911ed36f088a910"

--empty array--
Error: 2 - mcrypt_decrypt() expects parameter 2 to be string, array given, %s(%d)
string(0) ""

--int indexed array--
Error: 2 - mcrypt_decrypt() expects parameter 2 to be string, array given, %s(%d)
string(0) ""

--associative array--
Error: 2 - mcrypt_decrypt() expects parameter 2 to be string, array given, %s(%d)
string(0) ""

--nested arrays--
Error: 2 - mcrypt_decrypt() expects parameter 2 to be string, array given, %s(%d)
string(0) ""

--uppercase NULL--
string(32) "bfa2cb7240c8d2f6abeb34960c04f6d3"

--lowercase null--
string(32) "bfa2cb7240c8d2f6abeb34960c04f6d3"

--lowercase true--
string(32) "43a1ae011df36064589d06bc922ecd97"

--lowercase false--
string(32) "bfa2cb7240c8d2f6abeb34960c04f6d3"

--uppercase TRUE--
string(32) "43a1ae011df36064589d06bc922ecd97"

--uppercase FALSE--
string(32) "bfa2cb7240c8d2f6abeb34960c04f6d3"

--empty string DQ--
string(32) "bfa2cb7240c8d2f6abeb34960c04f6d3"

--empty string SQ--
string(32) "bfa2cb7240c8d2f6abeb34960c04f6d3"

--instance of classWithToString--
string(32) "478f9d080563835cc3136610802f1433"

--instance of classWithoutToString--
Error: 2 - mcrypt_decrypt() expects parameter 2 to be string, object given, %s(%d)
string(0) ""

--undefined var--
string(32) "bfa2cb7240c8d2f6abeb34960c04f6d3"

--unset var--
string(32) "bfa2cb7240c8d2f6abeb34960c04f6d3"

--resource--
Error: 2 - mcrypt_decrypt() expects parameter 2 to be string, resource given, %s(%d)
string(0) ""
===DONE===

