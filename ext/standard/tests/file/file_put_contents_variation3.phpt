--TEST--
Test file_put_contents() function : usage variation - different types for context.
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
$filename = 'FilePutContentsVar4.tmp';
$absFile = dirname(__FILE__).'/'.$filename;

$fileRes = fopen(__FILE__,'r');
$strContext = stream_context_create();

$data = "data to write";

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

      //non context resource
      'file resource' => $fileRes,

      //valid stream context
      'stream context' => $strContext,
);

// loop through each element of the array for context

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( file_put_contents($absFile, $data, null, $value) );
};

unlink($absFile);
fclose($fileRes);

?>
===DONE===
--EXPECTF--
*** Testing file_put_contents() : usage variation ***

--int 0--
Error: 2 - file_put_contents() expects parameter 4 to be resource, int given, %s(%d)
NULL

--int 1--
Error: 2 - file_put_contents() expects parameter 4 to be resource, int given, %s(%d)
NULL

--int 12345--
Error: 2 - file_put_contents() expects parameter 4 to be resource, int given, %s(%d)
NULL

--int -12345--
Error: 2 - file_put_contents() expects parameter 4 to be resource, int given, %s(%d)
NULL

--float 10.5--
Error: 2 - file_put_contents() expects parameter 4 to be resource, float given, %s(%d)
NULL

--float -10.5--
Error: 2 - file_put_contents() expects parameter 4 to be resource, float given, %s(%d)
NULL

--float 12.3456789000e10--
Error: 2 - file_put_contents() expects parameter 4 to be resource, float given, %s(%d)
NULL

--float -12.3456789000e10--
Error: 2 - file_put_contents() expects parameter 4 to be resource, float given, %s(%d)
NULL

--float .5--
Error: 2 - file_put_contents() expects parameter 4 to be resource, float given, %s(%d)
NULL

--empty array--
Error: 2 - file_put_contents() expects parameter 4 to be resource, array given, %s(%d)
NULL

--int indexed array--
Error: 2 - file_put_contents() expects parameter 4 to be resource, array given, %s(%d)
NULL

--associative array--
Error: 2 - file_put_contents() expects parameter 4 to be resource, array given, %s(%d)
NULL

--nested arrays--
Error: 2 - file_put_contents() expects parameter 4 to be resource, array given, %s(%d)
NULL

--uppercase NULL--
int(13)

--lowercase null--
int(13)

--lowercase true--
Error: 2 - file_put_contents() expects parameter 4 to be resource, bool given, %s(%d)
NULL

--lowercase false--
Error: 2 - file_put_contents() expects parameter 4 to be resource, bool given, %s(%d)
NULL

--uppercase TRUE--
Error: 2 - file_put_contents() expects parameter 4 to be resource, bool given, %s(%d)
NULL

--uppercase FALSE--
Error: 2 - file_put_contents() expects parameter 4 to be resource, bool given, %s(%d)
NULL

--empty string DQ--
Error: 2 - file_put_contents() expects parameter 4 to be resource, string given, %s(%d)
NULL

--empty string SQ--
Error: 2 - file_put_contents() expects parameter 4 to be resource, string given, %s(%d)
NULL

--string DQ--
Error: 2 - file_put_contents() expects parameter 4 to be resource, string given, %s(%d)
NULL

--string SQ--
Error: 2 - file_put_contents() expects parameter 4 to be resource, string given, %s(%d)
NULL

--mixed case string--
Error: 2 - file_put_contents() expects parameter 4 to be resource, string given, %s(%d)
NULL

--heredoc--
Error: 2 - file_put_contents() expects parameter 4 to be resource, string given, %s(%d)
NULL

--instance of classWithToString--
Error: 2 - file_put_contents() expects parameter 4 to be resource, object given, %s(%d)
NULL

--instance of classWithoutToString--
Error: 2 - file_put_contents() expects parameter 4 to be resource, object given, %s(%d)
NULL

--undefined var--
int(13)

--unset var--
int(13)

--file resource--
Error: 2 - file_put_contents(): supplied resource is not a valid Stream-Context resource, %s(%d)
int(13)

--stream context--
int(13)
===DONE===
