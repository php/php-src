--TEST--
Test touch() function : usage variation - different types for time
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (PHP_INT_SIZE != 8) die("skip this test is for 64-bit only");
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. Not for Windows');
}
?>
--FILE--
<?php
/* Prototype  : bool touch(string filename [, int time [, int atime]])
 * Description: Set modification time of file
 * Source code: ext/standard/filestat.c
 * Alias to functions:
 */

echo "*** Testing touch() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)
$filename = 'touchVar2.tmp';
$atime = 10;

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
      'float 12.3456789000e10' => 12.3456789000e10,
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

// loop through each element of the array for time

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( touch($filename, $value, $atime) );
};

unlink($filename);

?>
===DONE===
--EXPECTF--
*** Testing touch() : usage variation ***

--float 10.5--
bool(true)

--float 12.3456789000e10--
bool(true)

--float .5--
bool(true)

--empty array--
Error: 2 - touch() expects parameter 2 to be int, array given, %s(%d)
NULL

--int indexed array--
Error: 2 - touch() expects parameter 2 to be int, array given, %s(%d)
NULL

--associative array--
Error: 2 - touch() expects parameter 2 to be int, array given, %s(%d)
NULL

--nested arrays--
Error: 2 - touch() expects parameter 2 to be int, array given, %s(%d)
NULL

--uppercase NULL--
bool(true)

--lowercase null--
bool(true)

--lowercase true--
bool(true)

--lowercase false--
bool(true)

--uppercase TRUE--
bool(true)

--uppercase FALSE--
bool(true)

--empty string DQ--
Error: 2 - touch() expects parameter 2 to be int, string given, %s(%d)
NULL

--empty string SQ--
Error: 2 - touch() expects parameter 2 to be int, string given, %s(%d)
NULL

--string DQ--
Error: 2 - touch() expects parameter 2 to be int, string given, %s(%d)
NULL

--string SQ--
Error: 2 - touch() expects parameter 2 to be int, string given, %s(%d)
NULL

--mixed case string--
Error: 2 - touch() expects parameter 2 to be int, string given, %s(%d)
NULL

--heredoc--
Error: 2 - touch() expects parameter 2 to be int, string given, %s(%d)
NULL

--instance of classWithToString--
Error: 2 - touch() expects parameter 2 to be int, object given, %s(%d)
NULL

--instance of classWithoutToString--
Error: 2 - touch() expects parameter 2 to be int, object given, %s(%d)
NULL

--undefined var--
bool(true)

--unset var--
bool(true)
===DONE===
