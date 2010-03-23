--TEST--
Test file() function : first parameter variation 
--FILE--
<?php
/* Prototype  : array file(string filename [, int flags[, resource context]])
 * Description: Read entire file into an array 
 * Source code: ext/standard/file.c
 * Alias to functions: 
 */

echo "*** Testing file() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted
$flags = 0;
$context = stream_context_create();


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

// loop through each element of the array for filename

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( file($value, $flags, $context) );
};

?>
===DONE===
--EXPECTF--
*** Testing file() : usage variation ***

--int 0--
Error: 2 - file(0): failed to open stream: No such file or directory, %s(%d)
bool(false)

--int 1--
Error: 2 - file(1): failed to open stream: No such file or directory, %s(%d)
bool(false)

--int 12345--
Error: 2 - file(12345): failed to open stream: No such file or directory, %s(%d)
bool(false)

--int -12345--
Error: 2 - file(-2345): failed to open stream: No such file or directory, %s(%d)
bool(false)

--float 10.5--
Error: 2 - file(10.5): failed to open stream: No such file or directory, %s(%d)
bool(false)

--float -10.5--
Error: 2 - file(-10.5): failed to open stream: No such file or directory, %s(%d)
bool(false)

--float 12.3456789000e10--
Error: 2 - file(123456789000): failed to open stream: No such file or directory, %s(%d)
bool(false)

--float -12.3456789000e10--
Error: 2 - file(-123456789000): failed to open stream: No such file or directory, %s(%d)
bool(false)

--float .5--
Error: 2 - file(0.5): failed to open stream: No such file or directory, %s(%d)
bool(false)

--empty array--
Error: 2 - file() expects parameter 1 to be string, array given, %s(%d)
NULL

--int indexed array--
Error: 2 - file() expects parameter 1 to be string, array given, %s(%d)
NULL

--associative array--
Error: 2 - file() expects parameter 1 to be string, array given, %s(%d)
NULL

--nested arrays--
Error: 2 - file() expects parameter 1 to be string, array given, %s(%d)
NULL

--uppercase NULL--
Error: 2 - file(): Filename cannot be empty, %s(%d)
bool(false)

--lowercase null--
Error: 2 - file(): Filename cannot be empty, %s(%d)
bool(false)

--lowercase true--
Error: 2 - file(1): failed to open stream: No such file or directory, %s(%d)
bool(false)

--lowercase false--
Error: 2 - file(): Filename cannot be empty, %s(%d)
bool(false)

--uppercase TRUE--
Error: 2 - file(1): failed to open stream: No such file or directory, %s(%d)
bool(false)

--uppercase FALSE--
Error: 2 - file(): Filename cannot be empty, %s(%d)
bool(false)

--empty string DQ--
Error: 2 - file(): Filename cannot be empty, %s(%d)
bool(false)

--empty string SQ--
Error: 2 - file(): Filename cannot be empty, %s(%d)
bool(false)

--instance of classWithToString--
Error: 2 - file(Class A object): failed to open stream: %s, %s(%d)
bool(false)

--instance of classWithoutToString--
Error: 2 - file() expects parameter 1 to be string, object given, %s(%d)
NULL

--undefined var--
Error: 2 - file(): Filename cannot be empty, %s(%d)
bool(false)

--unset var--
Error: 2 - file(): Filename cannot be empty, %s(%d)
bool(false)
===DONE===
