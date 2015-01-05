--TEST--
Test long2ip() function : usage variation 
--SKIPIF--
<?php
if(substr(PHP_OS, 0, 3) == "WIN")
  die("skip don't run on Windows");
?> 
--FILE--
<?php
/* Prototype  : string long2ip(int proper_address)
 * Description: Converts an (IPv4) Internet network address into a string in Internet standard dotted format 
 * Source code: ext/standard/basic_functions.c
 * Alias to functions: 
 */

echo "*** Testing long2ip() : usage variation ***\n";

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

      // float data
      'float 10.5' => 10.5,
      'float -10.5' => -10.5,
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

      // string data
      'string DQ' => "string",
      'string SQ' => 'string',
      'mixed case string' => "sTrInG",
      'heredoc' => $heredoc,

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

// loop through each element of the array for proper_address

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( long2ip($value) );
};

fclose($res);

?>
===DONE===
--EXPECTF--
*** Testing long2ip() : usage variation ***

--float 10.5--
string(8) "0.0.0.10"

--float -10.5--
string(15) "255.255.255.246"

--float .5--
string(7) "0.0.0.0"

--empty array--
Error: 2 - long2ip() expects parameter 1 to be string, array given, %s(%d)
NULL

--int indexed array--
Error: 2 - long2ip() expects parameter 1 to be string, array given, %s(%d)
NULL

--associative array--
Error: 2 - long2ip() expects parameter 1 to be string, array given, %s(%d)
NULL

--nested arrays--
Error: 2 - long2ip() expects parameter 1 to be string, array given, %s(%d)
NULL

--uppercase NULL--
string(7) "0.0.0.0"

--lowercase null--
string(7) "0.0.0.0"

--lowercase true--
string(7) "0.0.0.1"

--lowercase false--
string(7) "0.0.0.0"

--uppercase TRUE--
string(7) "0.0.0.1"

--uppercase FALSE--
string(7) "0.0.0.0"

--empty string DQ--
string(7) "0.0.0.0"

--empty string SQ--
string(7) "0.0.0.0"

--string DQ--
string(7) "0.0.0.0"

--string SQ--
string(7) "0.0.0.0"

--mixed case string--
string(7) "0.0.0.0"

--heredoc--
string(7) "0.0.0.0"

--instance of classWithToString--
string(7) "0.0.0.0"

--instance of classWithoutToString--
Error: 2 - long2ip() expects parameter 1 to be string, object given, %s(%d)
NULL

--undefined var--
string(7) "0.0.0.0"

--unset var--
string(7) "0.0.0.0"

--resource--
Error: 2 - long2ip() expects parameter 1 to be string, resource given, %s(%d)
NULL
===DONE===
