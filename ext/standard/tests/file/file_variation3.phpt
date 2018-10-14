--TEST--
Test file() function : second parameter variation
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip this test is for 64-bit only");
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
$filename = __FILE__ . ".tmp";
$fd = fopen($filename, "w+");
fwrite($fd, "Line 1\nLine 2\nLine 3");
fclose($fd);

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

// loop through each element of the array for flags

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( file($filename, $value, $context) );
};

unlink(__FILE__ . ".tmp");

?>
===DONE===
--EXPECTF--
*** Testing file() : usage variation ***

--float 10.5--
array(3) {
  [0]=>
  string(6) "Line 1"
  [1]=>
  string(6) "Line 2"
  [2]=>
  string(6) "Line 3"
}

--float -10.5--
Error: 2 - file(): '-10' flag is not supported, %s(%d)
bool(false)

--float 12.3456789000e10--
Error: 2 - file(): '%i' flag is not supported, %s(%d)
bool(false)

--float -12.3456789000e10--
Error: 2 - file(): '%i' flag is not supported, %s(%d)
bool(false)

--float .5--
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}

--empty array--
Error: 2 - file() expects parameter 2 to be int, array given, %s(%d)
NULL

--int indexed array--
Error: 2 - file() expects parameter 2 to be int, array given, %s(%d)
NULL

--associative array--
Error: 2 - file() expects parameter 2 to be int, array given, %s(%d)
NULL

--nested arrays--
Error: 2 - file() expects parameter 2 to be int, array given, %s(%d)
NULL

--uppercase NULL--
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}

--lowercase null--
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}

--lowercase true--
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}

--lowercase false--
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}

--uppercase TRUE--
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}

--uppercase FALSE--
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}

--empty string DQ--
Error: 2 - file() expects parameter 2 to be int, string given, %s(%d)
NULL

--empty string SQ--
Error: 2 - file() expects parameter 2 to be int, string given, %s(%d)
NULL

--string DQ--
Error: 2 - file() expects parameter 2 to be int, string given, %s(%d)
NULL

--string SQ--
Error: 2 - file() expects parameter 2 to be int, string given, %s(%d)
NULL

--mixed case string--
Error: 2 - file() expects parameter 2 to be int, string given, %s(%d)
NULL

--heredoc--
Error: 2 - file() expects parameter 2 to be int, string given, %s(%d)
NULL

--instance of classWithToString--
Error: 2 - file() expects parameter 2 to be int, object given, %s(%d)
NULL

--instance of classWithoutToString--
Error: 2 - file() expects parameter 2 to be int, object given, %s(%d)
NULL

--undefined var--
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}

--unset var--
array(3) {
  [0]=>
  string(7) "Line 1
"
  [1]=>
  string(7) "Line 2
"
  [2]=>
  string(6) "Line 3"
}
===DONE===
