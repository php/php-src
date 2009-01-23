--TEST--
Test ip2long() function : usage variation 
--FILE--
<?php
/* Prototype  : int ip2long(string ip_address)
 * Description: Converts a string containing an (IPv4) Internet Protocol dotted address into a proper address 
 * Source code: ext/standard/basic_functions.c
 * Alias to functions: 
 */

echo "*** Testing ip2long() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)

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

// add arrays
$index_array = array (1, 2, 3);
$assoc_array = array ('one' => 1, 'two' => 2);

// resource
$res = fopen(__FILE__,'r');

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
      
      // resource
      'resource' => $res,
);

// loop through each element of the array for ip_address

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( ip2long($value) );
};

fclose($res);

?>
===DONE===
--EXPECTF--
*** Testing ip2long() : usage variation ***

--int 0--
int(0)

--int 1--
int(1)

--int 12345--
int(12345)

--int -12345--
bool(false)

--float 10.5--
int(167772165)

--float -10.5--
bool(false)

--float 12.3456789000e10--
bool(false)

--float -12.3456789000e10--
bool(false)

--float .5--
int(5)

--empty array--
Error: 8 - Array to string conversion, %s(%d)
bool(false)

--int indexed array--
Error: 8 - Array to string conversion, %s(%d)
bool(false)

--associative array--
Error: 8 - Array to string conversion, %s(%d)
bool(false)

--nested arrays--
Error: 8 - Array to string conversion, %s(%d)
bool(false)

--uppercase NULL--
bool(false)

--lowercase null--
bool(false)

--lowercase true--
int(1)

--lowercase false--
bool(false)

--uppercase TRUE--
int(1)

--uppercase FALSE--
bool(false)

--empty string DQ--
bool(false)

--empty string SQ--
bool(false)

--instance of classWithToString--
bool(false)

--instance of classWithoutToString--
Error: 4096 - Object of class classWithoutToString could not be converted to string, %s(%d)
Error: 8 - Object of class classWithoutToString to string conversion, %s(%d)
bool(false)

--undefined var--
bool(false)

--unset var--
bool(false)

--resource--
bool(false)
===DONE===