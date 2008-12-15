--TEST--
Test call_user_func_array() function : second parameter variation 
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
function test_func() {
}
$function_name = 'test_func';

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
);

// loop through each element of the array for parameters

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( call_user_func_array($function_name, $value) );
};

?>
===DONE===
--EXPECT--
*** Testing call_user_func_array() : usage variation ***

--int 0--
NULL

--int 1--
NULL

--int 12345--
NULL

--int -12345--
NULL

--float 10.5--
NULL

--float -10.5--
NULL

--float 12.3456789000e10--
NULL

--float -12.3456789000e10--
NULL

--float .5--
NULL

--uppercase NULL--
NULL

--lowercase null--
NULL

--lowercase true--
NULL

--lowercase false--
NULL

--uppercase TRUE--
NULL

--uppercase FALSE--
NULL

--empty string DQ--
NULL

--empty string SQ--
NULL

--string DQ--
NULL

--string SQ--
NULL

--mixed case string--
NULL

--heredoc--
NULL

--instance of classWithToString--
NULL

--instance of classWithoutToString--
NULL

--undefined var--
NULL

--unset var--
NULL
===DONE===
