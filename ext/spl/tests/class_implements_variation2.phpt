--TEST--
SPL: Test class_implements() function : variation
--FILE--
<?php
/* Prototype  : array class_implements(mixed what [, bool autoload ])
 * Description: Return all classes and interfaces implemented by SPL
 * Source code: ext/spl/php_spl.c
 * Alias to functions:
 */

echo "*** Testing class_implements() : variation ***\n";


// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	if (error_reporting() != 0) {
		// report non-silenced errors
		echo "Error: $err_no - $err_msg, $filename($linenum)\n";
	}
}
set_error_handler('test_error_handler');

// Initialise function arguments not being substituted (if any)
$class = 'Iterator';

//resource
$res = fopen(__FILE__,'r');

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

      // object data
      'instance of classWithToString' => new classWithToString(),
      'instance of classWithoutToString' => new classWithoutToString(),

      // undefined data
      'undefined var' => @$undefined_var,

      // unset data
      'unset var' => @$unset_var,

      //resource
      'resource' => $res,
);

// loop through each element of the array for pattern

foreach($inputs as $key =>$value) {
      echo "\n--$key--\n";
      var_dump( class_implements($class, $value) );
};

fclose($res);

?>
===DONE===
--EXPECTF--
*** Testing class_implements() : variation ***

--int 0--
array(1) {
  ["Traversable"]=>
  string(11) "Traversable"
}

--int 1--
array(1) {
  ["Traversable"]=>
  string(11) "Traversable"
}

--int 12345--
array(1) {
  ["Traversable"]=>
  string(11) "Traversable"
}

--int -12345--
array(1) {
  ["Traversable"]=>
  string(11) "Traversable"
}

--float 10.5--
array(1) {
  ["Traversable"]=>
  string(11) "Traversable"
}

--float -10.5--
array(1) {
  ["Traversable"]=>
  string(11) "Traversable"
}

--float 12.3456789000e10--
array(1) {
  ["Traversable"]=>
  string(11) "Traversable"
}

--float -12.3456789000e10--
array(1) {
  ["Traversable"]=>
  string(11) "Traversable"
}

--float .5--
array(1) {
  ["Traversable"]=>
  string(11) "Traversable"
}

--empty array--
Error: 2 - class_implements() expects parameter 2 to be boolean, array given, %s(%d)
bool(false)

--int indexed array--
Error: 2 - class_implements() expects parameter 2 to be boolean, array given, %s(%d)
bool(false)

--associative array--
Error: 2 - class_implements() expects parameter 2 to be boolean, array given, %s(%d)
bool(false)

--nested arrays--
Error: 2 - class_implements() expects parameter 2 to be boolean, array given, %s(%d)
bool(false)

--uppercase NULL--
array(1) {
  ["Traversable"]=>
  string(11) "Traversable"
}

--lowercase null--
array(1) {
  ["Traversable"]=>
  string(11) "Traversable"
}

--lowercase true--
array(1) {
  ["Traversable"]=>
  string(11) "Traversable"
}

--lowercase false--
array(1) {
  ["Traversable"]=>
  string(11) "Traversable"
}

--uppercase TRUE--
array(1) {
  ["Traversable"]=>
  string(11) "Traversable"
}

--uppercase FALSE--
array(1) {
  ["Traversable"]=>
  string(11) "Traversable"
}

--empty string DQ--
array(1) {
  ["Traversable"]=>
  string(11) "Traversable"
}

--empty string SQ--
array(1) {
  ["Traversable"]=>
  string(11) "Traversable"
}

--instance of classWithToString--
Error: 2 - class_implements() expects parameter 2 to be boolean, object given, %s(%d)
bool(false)

--instance of classWithoutToString--
Error: 2 - class_implements() expects parameter 2 to be boolean, object given, %s(%d)
bool(false)

--undefined var--
array(1) {
  ["Traversable"]=>
  string(11) "Traversable"
}

--unset var--
array(1) {
  ["Traversable"]=>
  string(11) "Traversable"
}

--resource--
Error: 2 - class_implements() expects parameter 2 to be boolean, resource given, %s(%d)
bool(false)
===DONE===
