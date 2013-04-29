--TEST--
Test array_multisort() function : usage variation - test sort order of all types
--FILE--
<?php
/* Prototype  : bool array_multisort(array ar1 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING|SORT_NATURAL|SORT_FLAG_CASE]] [, array ar2 [, SORT_ASC|SORT_DESC [, SORT_REGULAR|SORT_NUMERIC|SORT_STRING|SORT_NATURAL|SORT_FLAG_CASE]], ...])
 * Description: Sort multiple arrays at once similar to how ORDER BY clause works in SQL 
 * Source code: ext/standard/array.c
 * Alias to functions: 
 */

echo "*** Testing array_multisort() : usage variation  - test sort order of all types***\n";

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

var_dump(array_multisort($inputs, SORT_NUMERIC));
var_dump($inputs);

?>
===DONE===
--EXPECTF--
*** Testing array_multisort() : usage variation  - test sort order of all types***

Notice: Object of class classWithToString could not be converted to double in %sarray_multisort_variation9.php on line %d

Notice: Object of class classWithoutToString could not be converted to double in %sarray_multisort_variation9.php on line %d

Notice: Object of class classWithoutToString could not be converted to double in %sarray_multisort_variation9.php on line %d

Notice: Object of class classWithoutToString could not be converted to double in %sarray_multisort_variation9.php on line %d

Notice: Object of class classWithoutToString could not be converted to double in %sarray_multisort_variation9.php on line %d
bool(true)
array(10) {
  ["float -10.5"]=>
  float(-10.5)
  ["string DQ"]=>
  string(6) "string"
  ["undefined var"]=>
  NULL
  ["empty string DQ"]=>
  string(0) ""
  ["uppercase NULL"]=>
  NULL
  ["int 0"]=>
  int(0)
  [0]=>
  array(0) {
  }
  ["instance of classWithoutToString"]=>
  object(classWithoutToString)#2 (0) {
  }
  ["lowercase true"]=>
  bool(true)
  ["instance of classWithToString"]=>
  object(classWithToString)#1 (0) {
  }
}
===DONE===