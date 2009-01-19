--TEST--
Test gzopen() function : usage variation 
--SKIPIF--
<?php 
if (!extension_loaded("zlib")) {
	print "skip - zlib extension not loaded"; 
}	 
?>
--FILE--
<?php
/* Prototype  : resource gzopen(string filename, string mode [, int use_include_path])
 * Description: Open a .gz-file and return a .gz-file pointer 
 * Source code: ext/zlib/zlib.c
 * Alias to functions: 
 */

echo "*** Testing gzopen() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)
$filename = dirname(__FILE__)."/004.txt.gz";
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

// get a resource variable
$fp = fopen(__FILE__, "r");

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
      
      // resource variable
      'resource' => $fp      
);

// loop through each element of the array for use_include_path

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      $res = gzopen($filename, $mode, $value);
      var_dump($res);
      if ($res === true) {
         gzclose($res);
      }
};

fclose($fp);

?>
===DONE===
--EXPECTF--
*** Testing gzopen() : usage variation ***

--float 10.5--
resource(%d) of type (stream)

--float -10.5--
resource(%d) of type (stream)

--float 12.3456789000e10--
resource(%d) of type (stream)

--float -12.3456789000e10--
resource(%d) of type (stream)

--float .5--
resource(%d) of type (stream)

--empty array--
Error: 2 - gzopen() expects parameter 3 to be long, array given, %s(%d)
NULL

--int indexed array--
Error: 2 - gzopen() expects parameter 3 to be long, array given, %s(%d)
NULL

--associative array--
Error: 2 - gzopen() expects parameter 3 to be long, array given, %s(%d)
NULL

--nested arrays--
Error: 2 - gzopen() expects parameter 3 to be long, array given, %s(%d)
NULL

--uppercase NULL--
resource(%d) of type (stream)

--lowercase null--
resource(%d) of type (stream)

--lowercase true--
resource(%d) of type (stream)

--lowercase false--
resource(%d) of type (stream)

--uppercase TRUE--
resource(%d) of type (stream)

--uppercase FALSE--
resource(%d) of type (stream)

--empty string DQ--
Error: 2 - gzopen() expects parameter 3 to be long, string given, %s(%d)
NULL

--empty string SQ--
Error: 2 - gzopen() expects parameter 3 to be long, string given, %s(%d)
NULL

--string DQ--
Error: 2 - gzopen() expects parameter 3 to be long, string given, %s(%d)
NULL

--string SQ--
Error: 2 - gzopen() expects parameter 3 to be long, string given, %s(%d)
NULL

--mixed case string--
Error: 2 - gzopen() expects parameter 3 to be long, string given, %s(%d)
NULL

--heredoc--
Error: 2 - gzopen() expects parameter 3 to be long, string given, %s(%d)
NULL

--instance of classWithToString--
Error: 2 - gzopen() expects parameter 3 to be long, object given, %s(%d)
NULL

--instance of classWithoutToString--
Error: 2 - gzopen() expects parameter 3 to be long, object given, %s(%d)
NULL

--undefined var--
resource(%d) of type (stream)

--unset var--
resource(%d) of type (stream)

--resource--
Error: 2 - gzopen() expects parameter 3 to be long, resource given, %s(%d)
NULL
===DONE===
