--TEST--
Test call_user_func_array() function : first parameter variation 
--FILE--
<?php
/* Prototype  : mixed call_user_func_array(string function_name, array parameters)
 * Description: Call a user function which is the first parameter with the arguments contained in array 
 * Source code: ext/standard/basic_functions.c
 * Alias to functions: 
 */

echo "*** Testing call_user_func_array() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)
$parameters = array(1, 2);

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

// loop through each element of the array for function_name

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( call_user_func_array($value, $parameters) );
};

?>
===DONE===
--EXPECTF--
*** Testing call_user_func_array() : usage variation ***

--int 0--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, no array or string given, %s(%d)
NULL

--int 1--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, no array or string given, %s(%d)
NULL

--int 12345--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, no array or string given, %s(%d)
NULL

--int -12345--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, no array or string given, %s(%d)
NULL

--float 10.5--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, no array or string given, %s(%d)
NULL

--float -10.5--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, no array or string given, %s(%d)
NULL

--float 12.3456789000e10--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, no array or string given, %s(%d)
NULL

--float -12.3456789000e10--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, no array or string given, %s(%d)
NULL

--float .5--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, no array or string given, %s(%d)
NULL

--empty array--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, array must have exactly two members, %s(%d)
NULL

--int indexed array--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, array must have exactly two members, %s(%d)
NULL

--associative array--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, first array member is not a valid class name or object, %s(%d)
NULL

--nested arrays--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, array must have exactly two members, %s(%d)
NULL

--uppercase NULL--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, no array or string given, %s(%d)
NULL

--lowercase null--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, no array or string given, %s(%d)
NULL

--lowercase true--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, no array or string given, %s(%d)
NULL

--lowercase false--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, no array or string given, %s(%d)
NULL

--uppercase TRUE--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, no array or string given, %s(%d)
NULL

--uppercase FALSE--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, no array or string given, %s(%d)
NULL

--empty string DQ--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, function '' not found or invalid function name, %s(%d)
NULL

--empty string SQ--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, function '' not found or invalid function name, %s(%d)
NULL

--instance of classWithToString--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, no array or string given, %s(%d)
NULL

--instance of classWithoutToString--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, no array or string given, %s(%d)
NULL

--undefined var--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, no array or string given, %s(%d)
NULL

--unset var--
Error: 2 - call_user_func_array() expects parameter 1 to be a valid callback, no array or string given, %s(%d)
NULL
===DONE===
