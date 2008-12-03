--TEST--
Test mb_substitute_character() function : usage variation 
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_substitute_character') or die("skip mb_substitute_character() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : mixed mb_substitute_character([mixed substchar])
 * Description: Sets the current substitute_character or returns the current substitute_character 
 * Source code: ext/mbstring/mbstring.c
 * Alias to functions: 
 */

echo "*** Testing mb_substitute_character() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)

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
      
      // resource variable
      'resource' => $fp      
);

// loop through each element of the array for substchar

mb_internal_encoding('utf-8');
foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( mb_substitute_character($value) );
};

fclose($fp);

?>
===DONE===
--EXPECTF--
*** Testing mb_substitute_character() : usage variation ***

--int 0--
Error: 2 - mb_substitute_character(): Unknown character., %s(%d)
bool(false)

--int 1--
bool(true)

--int 12345--
bool(true)

--int -12345--
Error: 2 - mb_substitute_character(): Unknown character., %s(%d)
bool(false)

--float 10.5--
bool(true)

--float -10.5--
Error: 2 - mb_substitute_character(): Unknown character., %s(%d)
bool(false)

--float 12.3456789000e10--
Error: 2 - mb_substitute_character(): Unknown character., %s(%d)
bool(false)

--float -12.3456789000e10--
Error: 2 - mb_substitute_character(): Unknown character., %s(%d)
bool(false)

--float .5--
Error: 2 - mb_substitute_character(): Unknown character., %s(%d)
bool(false)

--empty array--
Error: 2 - mb_substitute_character(): Unknown character., %s(%d)
bool(false)

--int indexed array--
bool(true)

--associative array--
bool(true)

--nested arrays--
bool(true)

--uppercase NULL--
Error: 2 - mb_substitute_character(): Unknown character., %s(%d)
bool(false)

--lowercase null--
Error: 2 - mb_substitute_character(): Unknown character., %s(%d)
bool(false)

--lowercase true--
bool(true)

--lowercase false--
Error: 2 - mb_substitute_character(): Unknown character., %s(%d)
bool(false)

--uppercase TRUE--
bool(true)

--uppercase FALSE--
Error: 2 - mb_substitute_character(): Unknown character., %s(%d)
bool(false)

--empty string DQ--
Error: 2 - mb_substitute_character(): Unknown character., %s(%d)
bool(false)

--empty string SQ--
Error: 2 - mb_substitute_character(): Unknown character., %s(%d)
bool(false)

--string DQ--
Error: 2 - mb_substitute_character(): Unknown character., %s(%d)
bool(false)

--string SQ--
Error: 2 - mb_substitute_character(): Unknown character., %s(%d)
bool(false)

--mixed case string--
Error: 2 - mb_substitute_character(): Unknown character., %s(%d)
bool(false)

--heredoc--
Error: 2 - mb_substitute_character(): Unknown character., %s(%d)
bool(false)

--instance of classWithToString--
Error: 8 - Object of class classWithToString could not be converted to int, %s(%d)
bool(true)

--instance of classWithoutToString--
Error: 8 - Object of class classWithoutToString could not be converted to int, %s(%d)
bool(true)

--undefined var--
Error: 2 - mb_substitute_character(): Unknown character., %s(%d)
bool(false)

--unset var--
Error: 2 - mb_substitute_character(): Unknown character., %s(%d)
bool(false)

--resource--
bool(true)
===DONE===
