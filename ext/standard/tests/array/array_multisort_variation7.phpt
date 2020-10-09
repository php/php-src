--TEST--
Test array_multisort() function : usage variation - test sort order of all types
--FILE--
<?php
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

var_dump(array_multisort($inputs));
var_dump($inputs);

?>
--EXPECT--
*** Testing array_multisort() : usage variation  - test sort order of all types***
bool(true)
array(10) {
  ["float -10.5"]=>
  float(-10.5)
  ["int 0"]=>
  int(0)
  [0]=>
  array(0) {
  }
  ["uppercase NULL"]=>
  NULL
  ["empty string DQ"]=>
  string(0) ""
  ["undefined var"]=>
  NULL
  ["lowercase true"]=>
  bool(true)
  ["instance of classWithToString"]=>
  object(classWithToString)#1 (0) {
  }
  ["string DQ"]=>
  string(6) "string"
  ["instance of classWithoutToString"]=>
  object(classWithoutToString)#2 (0) {
  }
}
