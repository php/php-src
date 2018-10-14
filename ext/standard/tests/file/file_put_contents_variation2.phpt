--TEST--
Test file_put_contents() function : usage variation - different data types to write
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : int file_put_contents(string file, mixed data [, int flags [, resource context]])
 * Description: Write/Create a file with contents data and return the number of bytes written
 * Source code: ext/standard/file.c
 * Alias to functions:
 */

echo "*** Testing file_put_contents() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)

$filename = __DIR__ . '/fwriteVar5.tmp';



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

// loop through each element of the array for str

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      file_put_contents($filename, $value);
      readfile($filename);
};
unlink($filename);

?>
===DONE===
--EXPECTF--
*** Testing file_put_contents() : usage variation ***

--int 0--
0
--int 1--
1
--int 12345--
12345
--int -12345--
-2345
--float 10.5--
10.5
--float -10.5--
-10.5
--float 12.3456789000e10--
123456789000
--float -12.3456789000e10--
-123456789000
--float .5--
0.5
--empty array--

--int indexed array--
123
--associative array--
12
--nested arrays--
Error: 8 - Array to string conversion, %s(%d)
Error: 8 - Array to string conversion, %s(%d)
fooArrayArray
--uppercase NULL--

--lowercase null--

--lowercase true--
1
--lowercase false--

--uppercase TRUE--
1
--uppercase FALSE--

--empty string DQ--

--empty string SQ--

--instance of classWithToString--
Class A object
--instance of classWithoutToString--

--undefined var--

--unset var--
===DONE===
