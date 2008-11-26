--TEST--
Test umask() function : usage variation
--CREDITS--
Dave Kelsey <d_kelsey@uk.ibm.com>
--SKIPIF--
<?php
if (substr(PHP_OS, 0, 3) == 'WIN') {
    die('skip.. only for Non Windows');
}
?>
--FILE--
<?php
/* Prototype  : int umask([int mask])
 * Description: Return or change the umask 
 * Source code: ext/standard/file.c
 * Alias to functions: 
 */

echo "*** Testing umask() : usage variation ***\n";

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

// add arrays
$index_array = array (1, 2, 3);
$assoc_array = array ('one' => 1, 'two' => 2);

//array of values to iterate over
$inputs = array(
      //out of boundary integers
      'int -100' => -100,
      'int 99999' => 99999,

      // float data
      'float 10.5' => 10.5,
      'float -10.5' => -10.5,
      'float 12.3456789000e10' => 12.3456789000e10,
      //'float -12.3456789000e10' => -12.3456789000e10, different in p8
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

// loop through each element of the array for mask

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      umask(0);
      var_dump(umask($value));
      var_dump( umask());
};

?>
===DONE===
--EXPECTF--
*** Testing umask() : usage variation ***

--int -100--
int(0)
int(412)

--int 99999--
int(0)
int(159)

--float 10.5--
int(0)
int(10)

--float -10.5--
int(0)
int(502)

--float 12.3456789000e10--
int(0)
int(%d)

--float .5--
int(0)
int(0)

--empty array--
Error: 2 - umask() expects parameter 1 to be long, array given, %s(%d)
bool(false)
int(63)

--int indexed array--
Error: 2 - umask() expects parameter 1 to be long, array given, %s(%d)
bool(false)
int(63)

--associative array--
Error: 2 - umask() expects parameter 1 to be long, array given, %s(%d)
bool(false)
int(63)

--nested arrays--
Error: 2 - umask() expects parameter 1 to be long, array given, %s(%d)
bool(false)
int(63)

--uppercase NULL--
int(0)
int(0)

--lowercase null--
int(0)
int(0)

--lowercase true--
int(0)
int(1)

--lowercase false--
int(0)
int(0)

--uppercase TRUE--
int(0)
int(1)

--uppercase FALSE--
int(0)
int(0)

--empty string DQ--
Error: 2 - umask() expects parameter 1 to be long, string given, %s(%d)
bool(false)
int(63)

--empty string SQ--
Error: 2 - umask() expects parameter 1 to be long, string given, %s(%d)
bool(false)
int(63)

--string DQ--
Error: 2 - umask() expects parameter 1 to be long, string given, %s(%d)
bool(false)
int(63)

--string SQ--
Error: 2 - umask() expects parameter 1 to be long, string given, %s(%d)
bool(false)
int(63)

--mixed case string--
Error: 2 - umask() expects parameter 1 to be long, string given, %s(%d)
bool(false)
int(63)

--heredoc--
Error: 2 - umask() expects parameter 1 to be long, string given, %s(%d)
bool(false)
int(63)

--instance of classWithToString--
Error: 2 - umask() expects parameter 1 to be long, object given, %s(%d)
bool(false)
int(63)

--instance of classWithoutToString--
Error: 2 - umask() expects parameter 1 to be long, object given, %s(%d)
bool(false)
int(63)

--undefined var--
int(0)
int(0)

--unset var--
int(0)
int(0)
===DONE===

