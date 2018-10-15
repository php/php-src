--TEST--
Test parse_ini_file() function : usage variation
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--FILE--
<?php
/* Prototype  : array parse_ini_file(string filename [, bool process_sections])
 * Description: Parse configuration file
 * Source code: ext/standard/basic_functions.c
 * Alias to functions:
 */

echo "*** Testing parse_ini_file() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)
$filename = __FILE__."ParseIniFileVar5.ini";
$contents = "a=test";
@unlink($filename);
file_put_contents($filename, $contents);


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

// loop through each element of the array for process_sections

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( parse_ini_file($filename, $value) );
};

unlink($filename);
?>
===DONE===
--EXPECTF--
*** Testing parse_ini_file() : usage variation ***

--int 0--
array(1) {
  ["a"]=>
  string(4) "test"
}

--int 1--
array(1) {
  ["a"]=>
  string(4) "test"
}

--int 12345--
array(1) {
  ["a"]=>
  string(4) "test"
}

--int -12345--
array(1) {
  ["a"]=>
  string(4) "test"
}

--float 10.5--
array(1) {
  ["a"]=>
  string(4) "test"
}

--float -10.5--
array(1) {
  ["a"]=>
  string(4) "test"
}

--float 12.3456789000e10--
array(1) {
  ["a"]=>
  string(4) "test"
}

--float -12.3456789000e10--
array(1) {
  ["a"]=>
  string(4) "test"
}

--float .5--
array(1) {
  ["a"]=>
  string(4) "test"
}

--empty array--
Error: 2 - parse_ini_file() expects parameter 2 to be bool, array given, %s(%d)
bool(false)

--int indexed array--
Error: 2 - parse_ini_file() expects parameter 2 to be bool, array given, %s(%d)
bool(false)

--associative array--
Error: 2 - parse_ini_file() expects parameter 2 to be bool, array given, %s(%d)
bool(false)

--nested arrays--
Error: 2 - parse_ini_file() expects parameter 2 to be bool, array given, %s(%d)
bool(false)

--uppercase NULL--
array(1) {
  ["a"]=>
  string(4) "test"
}

--lowercase null--
array(1) {
  ["a"]=>
  string(4) "test"
}

--empty string DQ--
array(1) {
  ["a"]=>
  string(4) "test"
}

--empty string SQ--
array(1) {
  ["a"]=>
  string(4) "test"
}

--string DQ--
array(1) {
  ["a"]=>
  string(4) "test"
}

--string SQ--
array(1) {
  ["a"]=>
  string(4) "test"
}

--mixed case string--
array(1) {
  ["a"]=>
  string(4) "test"
}

--heredoc--
array(1) {
  ["a"]=>
  string(4) "test"
}

--instance of classWithToString--
Error: 2 - parse_ini_file() expects parameter 2 to be bool, object given, %s(%d)
bool(false)

--instance of classWithoutToString--
Error: 2 - parse_ini_file() expects parameter 2 to be bool, object given, %s(%d)
bool(false)

--undefined var--
array(1) {
  ["a"]=>
  string(4) "test"
}

--unset var--
array(1) {
  ["a"]=>
  string(4) "test"
}
===DONE===
