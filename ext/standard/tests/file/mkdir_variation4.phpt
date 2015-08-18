--TEST--
Test mkdir() function : usage variation: different types for context
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : bool mkdir(string pathname [, int mode [, bool recursive [, resource context]]])
 * Description: Create a directory 
 * Source code: ext/standard/file.c
 * Alias to functions: 
 */

echo "*** Testing mkdir() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)
$pathname = 'mkdirVar4.tmp';
$mode = 0777;
$recursive = false;

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

$fileRes = fopen(__FILE__,'r');

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
      
      // non stream context resource
      'file resource' => $fileRes,
);

// loop through each element of the array for context

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      $h =  mkdir($pathname, $mode, $recursive, $value);
      if ($h === true) {
         echo "Directory created\n";
         rmdir($pathname);
      }
};

fclose($fileRes);

?>
===DONE===
--EXPECTF--
*** Testing mkdir() : usage variation ***

--int 0--
Error: 2 - mkdir() expects parameter 4 to be resource, integer given, %s(%d)

--int 1--
Error: 2 - mkdir() expects parameter 4 to be resource, integer given, %s(%d)

--int 12345--
Error: 2 - mkdir() expects parameter 4 to be resource, integer given, %s(%d)

--int -12345--
Error: 2 - mkdir() expects parameter 4 to be resource, integer given, %s(%d)

--float 10.5--
Error: 2 - mkdir() expects parameter 4 to be resource, float given, %s(%d)

--float -10.5--
Error: 2 - mkdir() expects parameter 4 to be resource, float given, %s(%d)

--float 12.3456789000e10--
Error: 2 - mkdir() expects parameter 4 to be resource, float given, %s(%d)

--float -12.3456789000e10--
Error: 2 - mkdir() expects parameter 4 to be resource, float given, %s(%d)

--float .5--
Error: 2 - mkdir() expects parameter 4 to be resource, float given, %s(%d)

--empty array--
Error: 2 - mkdir() expects parameter 4 to be resource, array given, %s(%d)

--int indexed array--
Error: 2 - mkdir() expects parameter 4 to be resource, array given, %s(%d)

--associative array--
Error: 2 - mkdir() expects parameter 4 to be resource, array given, %s(%d)

--nested arrays--
Error: 2 - mkdir() expects parameter 4 to be resource, array given, %s(%d)

--uppercase NULL--
Error: 2 - mkdir() expects parameter 4 to be resource, null given, %s(%d)

--lowercase null--
Error: 2 - mkdir() expects parameter 4 to be resource, null given, %s(%d)

--lowercase true--
Error: 2 - mkdir() expects parameter 4 to be resource, boolean given, %s(%d)

--lowercase false--
Error: 2 - mkdir() expects parameter 4 to be resource, boolean given, %s(%d)

--uppercase TRUE--
Error: 2 - mkdir() expects parameter 4 to be resource, boolean given, %s(%d)

--uppercase FALSE--
Error: 2 - mkdir() expects parameter 4 to be resource, boolean given, %s(%d)

--empty string DQ--
Error: 2 - mkdir() expects parameter 4 to be resource, string given, %s(%d)

--empty string SQ--
Error: 2 - mkdir() expects parameter 4 to be resource, string given, %s(%d)

--string DQ--
Error: 2 - mkdir() expects parameter 4 to be resource, string given, %s(%d)

--string SQ--
Error: 2 - mkdir() expects parameter 4 to be resource, string given, %s(%d)

--mixed case string--
Error: 2 - mkdir() expects parameter 4 to be resource, string given, %s(%d)

--heredoc--
Error: 2 - mkdir() expects parameter 4 to be resource, string given, %s(%d)

--instance of classWithToString--
Error: 2 - mkdir() expects parameter 4 to be resource, object given, %s(%d)

--instance of classWithoutToString--
Error: 2 - mkdir() expects parameter 4 to be resource, object given, %s(%d)

--undefined var--
Error: 2 - mkdir() expects parameter 4 to be resource, null given, %s(%d)

--unset var--
Error: 2 - mkdir() expects parameter 4 to be resource, null given, %s(%d)

--file resource--
Error: 2 - mkdir(): supplied resource is not a valid Stream-Context resource, %s(%d)
Directory created
===DONE===