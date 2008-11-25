--TEST--
Test chmod() function : second parameter variation 
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : bool chmod(string filename, int mode)
 * Description: Change file mode 
 * Source code: ext/standard/filestat.c
 * Alias to functions: 
 */

echo "*** Testing chmod() : usage variation ***\n";

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
fclose($fd);

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

// loop through each element of the array for mode

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( chmod($filename, $value) );
};

chmod($filename, 0777);
unlink($filename);

?>
===DONE===
--EXPECTF--
*** Testing chmod() : usage variation ***

--float 10.5--
bool(true)

--float -10.5--
bool(true)

--float 12.3456789000e10--
bool(true)

--float -12.3456789000e10--
bool(true)

--float .5--
bool(true)

--empty array--
bool(true)

--int indexed array--
bool(true)

--associative array--
bool(true)

--nested arrays--
bool(true)

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
bool(true)

--empty string SQ--
bool(true)

--string DQ--
bool(true)

--string SQ--
bool(true)

--mixed case string--
bool(true)

--heredoc--
bool(true)

--instance of classWithToString--
Error: 8 - Object of class classWithToString could not be converted to int, %s(%d)
bool(true)

--instance of classWithoutToString--
Error: 8 - Object of class classWithoutToString could not be converted to int, %s(%d)
bool(true)

--undefined var--
bool(true)

--unset var--
bool(true)
===DONE===
