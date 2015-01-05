--TEST--
Test basename() function : second parameter type variation
--FILE--
<?php
/* Prototype  : string basename(string path [, string suffix])
 * Description: Returns the filename component of the path 
 * Source code: ext/standard/string.c
 * Alias to functions: 
 */

echo "*** Testing basename() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted
$path = 'path';

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
);

// loop through each element of the array for suffix

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( basename($path, $value) );
};

?>
===DONE===
--EXPECTF--
*** Testing basename() : usage variation ***

--int 0--
string(4) "path"

--int 1--
string(4) "path"

--int 12345--
string(4) "path"

--int -12345--
string(4) "path"

--float 10.5--
string(4) "path"

--float -10.5--
string(4) "path"

--float 12.3456789000e10--
string(4) "path"

--float -12.3456789000e10--
string(4) "path"

--float .5--
string(4) "path"

--empty array--
Error: 2 - basename() expects parameter 2 to be string, array given, %s(%d)
NULL

--int indexed array--
Error: 2 - basename() expects parameter 2 to be string, array given, %s(%d)
NULL

--associative array--
Error: 2 - basename() expects parameter 2 to be string, array given, %s(%d)
NULL

--nested arrays--
Error: 2 - basename() expects parameter 2 to be string, array given, %s(%d)
NULL

--uppercase NULL--
string(4) "path"

--lowercase null--
string(4) "path"

--lowercase true--
string(4) "path"

--lowercase false--
string(4) "path"

--uppercase TRUE--
string(4) "path"

--uppercase FALSE--
string(4) "path"

--empty string DQ--
string(4) "path"

--empty string SQ--
string(4) "path"

--instance of classWithToString--
string(4) "path"

--instance of classWithoutToString--
Error: 2 - basename() expects parameter 2 to be string, object given, %s(%d)
NULL

--undefined var--
string(4) "path"

--unset var--
string(4) "path"
===DONE===
