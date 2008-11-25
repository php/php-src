--TEST--
Test fopen() function : usage variation different datatypes for use_include_path
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : resource fopen(string filename, string mode [, bool use_include_path [, resource context]])
 * Description: Open a file or a URL and return a file pointer 
 * Source code: ext/standard/file.c
 * Alias to functions: 
 */

echo "*** Testing fopen() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)
$filename = __FILE__;
$mode = 'r';


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
      $h = fopen($filename, $mode, $value);
      if ($h !== false) {
         echo "ok\n";
         fclose($h);
      }
      else {
         var_dump($h);
      }
};

?>
===DONE===
--EXPECTF--
*** Testing fopen() : usage variation ***

--int 0--
ok

--int 1--
ok

--int 12345--
ok

--int -12345--
ok

--float 10.5--
ok

--float -10.5--
ok

--float 12.3456789000e10--
ok

--float -12.3456789000e10--
ok

--float .5--
ok

--empty array--
Error: 2 - fopen() expects parameter 3 to be boolean, array given, %s(%d)
bool(false)

--int indexed array--
Error: 2 - fopen() expects parameter 3 to be boolean, array given, %s(%d)
bool(false)

--associative array--
Error: 2 - fopen() expects parameter 3 to be boolean, array given, %s(%d)
bool(false)

--nested arrays--
Error: 2 - fopen() expects parameter 3 to be boolean, array given, %s(%d)
bool(false)

--uppercase NULL--
ok

--lowercase null--
ok

--lowercase true--
ok

--lowercase false--
ok

--uppercase TRUE--
ok

--uppercase FALSE--
ok

--empty string DQ--
ok

--empty string SQ--
ok

--string DQ--
ok

--string SQ--
ok

--mixed case string--
ok

--heredoc--
ok

--instance of classWithToString--
Error: 2 - fopen() expects parameter 3 to be boolean, object given, %s(%d)
bool(false)

--instance of classWithoutToString--
Error: 2 - fopen() expects parameter 3 to be boolean, object given, %s(%d)
bool(false)

--undefined var--
ok

--unset var--
ok
===DONE===
