--TEST--
Test array_multisort() function : usage variation - test sort order of all types
--FILE--
<?php
/* Prototype  : bool array_multisort(array ar1 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING]] [, array ar2 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING]], ...])
 * Description: Sort multiple arrays at once similar to how ORDER BY clause works in SQL 
 * Source code: ext/standard/array.c
 * Alias to functions: 
 */

echo "*** Testing array_multisort() : usage variation  - test sort order of all types***\n";

// Define error handler
function test_error_handler($err_no, $err_msg, $filename, $linenum, $vars) {
	// We're testing sort order not errors so ignore.
}
set_error_handler('test_error_handler');

// define some classes
class classWithToString {
	public function __toString() {
		return "Class A object";
	}
}

class classWithoutToString { }

$inputs = array(
      'int 0' => 0,
      'float -10.5' => -10.5,
      array(),
      'uppercase NULL' => NULL,
      'lowercase true' => true,
      'empty string DQ' => "",
      'string DQ' => "string",
      'instance of classWithToString' => new classWithToString(),
      'instance of classWithoutToString' => new classWithoutToString(),
      'undefined var' => @$undefined_var,
);

var_dump(array_multisort($inputs, SORT_STRING));
var_dump($inputs);

?>
===DONE===
--EXPECTF--
*** Testing array_multisort() : usage variation  - test sort order of all types***
bool(true)
array(10) {
  ["empty string DQ"]=>
  string(0) ""
  ["uppercase NULL"]=>
  NULL
  ["undefined var"]=>
  NULL
  ["instance of classWithoutToString"]=>
  object(classWithoutToString)#2 (0) {
  }
  ["float -10.5"]=>
  float(-10.5)
  ["int 0"]=>
  int(0)
  ["lowercase true"]=>
  bool(true)
  [0]=>
  array(0) {
  }
  ["instance of classWithToString"]=>
  object(classWithToString)#1 (0) {
  }
  ["string DQ"]=>
  string(6) "string"
}
===DONE===
