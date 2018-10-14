--TEST--
Test readfile() function : usage variation
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : int readfile(string filename [, bool use_include_path[, resource context]])
 * Description: Output a file or a URL
 * Source code: ext/standard/file.c
 * Alias to functions:
 */

echo "*** Testing readfile() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)
$filename = 'readFileVar4.tmp';
$use_include_path = false;
$h = fopen($filename,'wb');
fwrite($h, "testing readfile");
fclose($h);


$fileRes = fopen(__FILE__, 'r');

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

      // file resource
      'file resource' => $fileRes,
);

// loop through each element of the array for context

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      $h = readfile($filename, $use_include_path, $value);
      if ($h == false) {
         echo "file not read\n";
      }
      else {
         echo "\n";
      }
};

unlink($filename);
fclose($fileRes);

?>
===DONE===
--EXPECTF--
*** Testing readfile() : usage variation ***

--int 0--
Error: 2 - readfile() expects parameter 3 to be resource, integer given, %s(%d)
file not read

--int 1--
Error: 2 - readfile() expects parameter 3 to be resource, integer given, %s(%d)
file not read

--int 12345--
Error: 2 - readfile() expects parameter 3 to be resource, integer given, %s(%d)
file not read

--int -12345--
Error: 2 - readfile() expects parameter 3 to be resource, integer given, %s(%d)
file not read

--float 10.5--
Error: 2 - readfile() expects parameter 3 to be resource, float given, %s(%d)
file not read

--float -10.5--
Error: 2 - readfile() expects parameter 3 to be resource, float given, %s(%d)
file not read

--float 12.3456789000e10--
Error: 2 - readfile() expects parameter 3 to be resource, float given, %s(%d)
file not read

--float -12.3456789000e10--
Error: 2 - readfile() expects parameter 3 to be resource, float given, %s(%d)
file not read

--float .5--
Error: 2 - readfile() expects parameter 3 to be resource, float given, %s(%d)
file not read

--empty array--
Error: 2 - readfile() expects parameter 3 to be resource, array given, %s(%d)
file not read

--int indexed array--
Error: 2 - readfile() expects parameter 3 to be resource, array given, %s(%d)
file not read

--associative array--
Error: 2 - readfile() expects parameter 3 to be resource, array given, %s(%d)
file not read

--nested arrays--
Error: 2 - readfile() expects parameter 3 to be resource, array given, %s(%d)
file not read

--uppercase NULL--
testing readfile

--lowercase null--
testing readfile

--lowercase true--
Error: 2 - readfile() expects parameter 3 to be resource, boolean given, %s(%d)
file not read

--lowercase false--
Error: 2 - readfile() expects parameter 3 to be resource, boolean given, %s(%d)
file not read

--uppercase TRUE--
Error: 2 - readfile() expects parameter 3 to be resource, boolean given, %s(%d)
file not read

--uppercase FALSE--
Error: 2 - readfile() expects parameter 3 to be resource, boolean given, %s(%d)
file not read

--empty string DQ--
Error: 2 - readfile() expects parameter 3 to be resource, string given, %s(%d)
file not read

--empty string SQ--
Error: 2 - readfile() expects parameter 3 to be resource, string given, %s(%d)
file not read

--string DQ--
Error: 2 - readfile() expects parameter 3 to be resource, string given, %s(%d)
file not read

--string SQ--
Error: 2 - readfile() expects parameter 3 to be resource, string given, %s(%d)
file not read

--mixed case string--
Error: 2 - readfile() expects parameter 3 to be resource, string given, %s(%d)
file not read

--heredoc--
Error: 2 - readfile() expects parameter 3 to be resource, string given, %s(%d)
file not read

--instance of classWithToString--
Error: 2 - readfile() expects parameter 3 to be resource, object given, %s(%d)
file not read

--instance of classWithoutToString--
Error: 2 - readfile() expects parameter 3 to be resource, object given, %s(%d)
file not read

--undefined var--
testing readfile

--unset var--
testing readfile

--file resource--
Error: 2 - readfile(): supplied resource is not a valid Stream-Context resource, %s(%d)
testing readfile
===DONE===
