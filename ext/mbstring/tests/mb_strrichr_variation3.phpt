--TEST--
Test mb_strrichr() function : usage variation - different values for part
--SKIPIF--
<?php
extension_loaded('mbstring') or die('skip');
function_exists('mb_strrichr') or die("skip mb_strrichr() is not available in this build");
?>
--FILE--
<?php
/* Prototype  : string mb_strrichr(string haystack, string needle[, bool part[, string encoding]])
 * Description: Finds the last occurrence of a character in a string within another, case insensitive 
 * Source code: ext/mbstring/mbstring.c
 * Alias to functions: 
 */
 
echo "*** Testing mb_strrichr() : usage variation ***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)
$haystack = b'string_val';
$needle = b'_';
$encoding = 'utf-8';

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

// loop through each element of the array for part

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      $res = mb_strrichr($haystack, $needle, $value, $encoding);
      if ($res === false) {
         var_dump($res);
      }
      else {
         var_dump(bin2hex($res));
      }         
};

fclose($fp);

?>
===DONE===
--EXPECTF--
*** Testing mb_strrichr() : usage variation ***

--int 0--
string(8) "5f76616c"

--int 1--
string(12) "737472696e67"

--int 12345--
string(12) "737472696e67"

--int -12345--
string(12) "737472696e67"

--float 10.5--
string(12) "737472696e67"

--float -10.5--
string(12) "737472696e67"

--float 12.3456789000e10--
string(12) "737472696e67"

--float -12.3456789000e10--
string(12) "737472696e67"

--float .5--
string(12) "737472696e67"

--empty array--
Error: 2 - mb_strrichr() expects parameter 3 to be boolean, array given, %s(%d)
bool(false)

--int indexed array--
Error: 2 - mb_strrichr() expects parameter 3 to be boolean, array given, %s(%d)
bool(false)

--associative array--
Error: 2 - mb_strrichr() expects parameter 3 to be boolean, array given, %s(%d)
bool(false)

--nested arrays--
Error: 2 - mb_strrichr() expects parameter 3 to be boolean, array given, %s(%d)
bool(false)

--uppercase NULL--
string(8) "5f76616c"

--lowercase null--
string(8) "5f76616c"

--lowercase true--
string(12) "737472696e67"

--lowercase false--
string(8) "5f76616c"

--uppercase TRUE--
string(12) "737472696e67"

--uppercase FALSE--
string(8) "5f76616c"

--empty string DQ--
string(8) "5f76616c"

--empty string SQ--
string(8) "5f76616c"

--string DQ--
string(12) "737472696e67"

--string SQ--
string(12) "737472696e67"

--mixed case string--
string(12) "737472696e67"

--heredoc--
string(12) "737472696e67"

--instance of classWithToString--
Error: 2 - mb_strrichr() expects parameter 3 to be boolean, object given, %s(%d)
bool(false)

--instance of classWithoutToString--
Error: 2 - mb_strrichr() expects parameter 3 to be boolean, object given, %s(%d)
bool(false)

--undefined var--
string(8) "5f76616c"

--unset var--
string(8) "5f76616c"

--resource--
Error: 2 - mb_strrichr() expects parameter 3 to be boolean, resource given, %s(%d)
bool(false)
===DONE===
