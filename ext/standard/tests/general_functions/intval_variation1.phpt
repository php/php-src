--TEST--
Test intval() function : usage variation
--FILE--
<?php
/* Prototype  : int intval(mixed var [, int base])
 * Description: Get the integer value of a variable using the optional base for the conversion
 * Source code: ext/standard/type.c
 * Alias to functions:
 */

echo "*** Testing intval() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)
$base = 10;

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
      'float 12.3456789e5' => 12.3456789e5,
      'float -12.3456789e5' => -12.3456789e5,
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

// loop through each element of the array for var

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( intval($value, $base) );
};

?>
===DONE===
--EXPECTF--
*** Testing intval() : usage variation ***

--int 0--
int(0)

--int 1--
int(1)

--int 12345--
int(12345)

--int -12345--
int(-2345)

--float 10.5--
int(10)

--float -10.5--
int(-10)

--float 12.3456789e5--
int(1234567)

--float -12.3456789e5--
int(-1234567)

--float .5--
int(0)

--empty array--
int(0)

--int indexed array--
int(1)

--associative array--
int(1)

--nested arrays--
int(1)

--uppercase NULL--
int(0)

--lowercase null--
int(0)

--lowercase true--
int(1)

--lowercase false--
int(0)

--uppercase TRUE--
int(1)

--uppercase FALSE--
int(0)

--empty string DQ--
int(0)

--empty string SQ--
int(0)

--string DQ--
int(0)

--string SQ--
int(0)

--mixed case string--
int(0)

--heredoc--
int(0)

--instance of classWithToString--
Error: 8 - Object of class classWithToString could not be converted to int, %s(%d)
int(1)

--instance of classWithoutToString--
Error: 8 - Object of class classWithoutToString could not be converted to int, %s(%d)
int(1)

--undefined var--
int(0)

--unset var--
int(0)
===DONE===
