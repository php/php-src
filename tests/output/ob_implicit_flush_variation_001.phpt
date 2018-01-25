--TEST--
Test ob_implicit_flush() function : usage variation
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64-bit only");
--FILE--
<?php
/* Prototype  : void ob_implicit_flush([int flag])
 * Description: Turn implicit flush on/off and is equivalent to calling flush() after every output call
 * Source code: main/output.c
 * Alias to functions:
 */

echo "*** Testing ob_implicit_flush() : usage variation ***\n";

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

//array of values to iterate over
$inputs = array(

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

// loop through each element of the array for flag

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( ob_implicit_flush($value) );
};

?>
--EXPECTF--
*** Testing ob_implicit_flush() : usage variation ***

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

--empty array--
Error: 2 - ob_implicit_flush() expects parameter 1 to be integer, array given, %s(97)
NULL

--int indexed array--
Error: 2 - ob_implicit_flush() expects parameter 1 to be integer, array given, %s(97)
NULL

--associative array--
Error: 2 - ob_implicit_flush() expects parameter 1 to be integer, array given, %s(97)
NULL

--nested arrays--
Error: 2 - ob_implicit_flush() expects parameter 1 to be integer, array given, %s(97)
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
Error: 2 - ob_implicit_flush() expects parameter 1 to be integer, string given, %s(97)
NULL

--empty string SQ--
Error: 2 - ob_implicit_flush() expects parameter 1 to be integer, string given, %s(97)
NULL

--string DQ--
Error: 2 - ob_implicit_flush() expects parameter 1 to be integer, string given, %s(97)
NULL

--string SQ--
Error: 2 - ob_implicit_flush() expects parameter 1 to be integer, string given, %s(97)
NULL

--mixed case string--
Error: 2 - ob_implicit_flush() expects parameter 1 to be integer, string given, %s(97)
NULL

--heredoc--
Error: 2 - ob_implicit_flush() expects parameter 1 to be integer, string given, %s(97)
NULL

--instance of classWithToString--
Error: 2 - ob_implicit_flush() expects parameter 1 to be integer, object given, %s(97)
NULL

--instance of classWithoutToString--
Error: 2 - ob_implicit_flush() expects parameter 1 to be integer, object given, %s(97)
NULL

--undefined var--
NULL

--unset var--
NULL
