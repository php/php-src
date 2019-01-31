--TEST--
Test file_get_contents() function : usage variation - different type for use_include_path
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : string file_get_contents(string filename [, bool use_include_path [, resource context [, long offset [, long maxlen]]]])
 * Description: Read the entire file into a string
 * Source code: ext/standard/file.c
 * Alias to functions:
 */

echo "*** Testing file_get_contents() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)
$filename = 'FileGetContentsVar3.tmp';
$absFile = dirname(__FILE__).'/'.$filename;
$h = fopen($absFile,"w");
fwrite($h, "contents read");
fclose($h);




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

// loop through each element of the array for use_include_path

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( file_get_contents($absFile, $value) );
};

unlink($absFile);

?>
===DONE===
--EXPECTF--
*** Testing file_get_contents() : usage variation ***

--int 0--
string(13) "contents read"

--int 1--
string(13) "contents read"

--int 12345--
string(%d) "contents read"

--int -12345--
string(%d) "contents read"

--float 10.5--
string(%d) "contents read"

--float -10.5--
string(%d) "contents read"

--float 12.3456789000e10--
string(%d) "contents read"

--float -12.3456789000e10--
string(%d) "contents read"

--float .5--
string(%d) "contents read"

--empty array--
Error: 2 - file_get_contents() expects parameter 2 to be bool, array given, %s(%d)
NULL

--int indexed array--
Error: 2 - file_get_contents() expects parameter 2 to be bool, array given, %s(%d)
NULL

--associative array--
Error: 2 - file_get_contents() expects parameter 2 to be bool, array given, %s(%d)
NULL

--nested arrays--
Error: 2 - file_get_contents() expects parameter 2 to be bool, array given, %s(%d)
NULL

--uppercase NULL--
string(%d) "contents read"

--lowercase null--
string(%d) "contents read"

--lowercase true--
string(%d) "contents read"

--lowercase false--
string(%d) "contents read"

--uppercase TRUE--
string(%d) "contents read"

--uppercase FALSE--
string(%d) "contents read"

--empty string DQ--
string(%d) "contents read"

--empty string SQ--
string(%d) "contents read"

--string DQ--
string(%d) "contents read"

--string SQ--
string(%d) "contents read"

--mixed case string--
string(%d) "contents read"

--heredoc--
string(%d) "contents read"

--instance of classWithToString--
Error: 2 - file_get_contents() expects parameter 2 to be bool, object given, %s(%d)
NULL

--instance of classWithoutToString--
Error: 2 - file_get_contents() expects parameter 2 to be bool, object given, %s(%d)
NULL

--undefined var--
string(%d) "contents read"

--unset var--
string(%d) "contents read"
===DONE===
