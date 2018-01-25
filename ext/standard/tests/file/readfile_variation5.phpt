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
$filename = 'readFileVar5.tmp';
$use_include_path = false;
$h = fopen($filename,'wb');
fwrite($h, "testing readfile");
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
      $res = readfile($filename, $value);
      if ($res == false) {
         echo "File not read\n";
      }
      else {
         echo "\n";
      }
};

unlink($filename);

?>
===DONE===
--EXPECTF--
*** Testing readfile() : usage variation ***

--int 0--
testing readfile

--int 1--
testing readfile

--int 12345--
testing readfile

--int -12345--
testing readfile

--float 10.5--
testing readfile

--float -10.5--
testing readfile

--float 12.3456789000e10--
testing readfile

--float -12.3456789000e10--
testing readfile

--float .5--
testing readfile

--empty array--
Error: 2 - readfile() expects parameter 2 to be boolean, array given, %s(%d)
File not read

--int indexed array--
Error: 2 - readfile() expects parameter 2 to be boolean, array given, %s(%d)
File not read

--associative array--
Error: 2 - readfile() expects parameter 2 to be boolean, array given, %s(%d)
File not read

--nested arrays--
Error: 2 - readfile() expects parameter 2 to be boolean, array given, %s(%d)
File not read

--uppercase NULL--
testing readfile

--lowercase null--
testing readfile

--lowercase true--
testing readfile

--lowercase false--
testing readfile

--uppercase TRUE--
testing readfile

--uppercase FALSE--
testing readfile

--empty string DQ--
testing readfile

--empty string SQ--
testing readfile

--string DQ--
testing readfile

--string SQ--
testing readfile

--mixed case string--
testing readfile

--heredoc--
testing readfile

--instance of classWithToString--
Error: 2 - readfile() expects parameter 2 to be boolean, object given, %s(%d)
File not read

--instance of classWithoutToString--
Error: 2 - readfile() expects parameter 2 to be boolean, object given, %s(%d)
File not read

--undefined var--
testing readfile

--unset var--
testing readfile
===DONE===
