--TEST--
Test array_fill_keys() function : variation of parameter
--FILE--
<?php
/* Testing with unexpected argument types */

echo "*** Testing array_fill_keys() : parameter variations ***\n";

$fp = fopen(__FILE__, "r");
$bool = false;
$float = 2.4;
$array = array("one");
$nullVal = null;
$unset_var = 10;
unset ($unset_var);


class classA {
  public function __toString() { return "Class A object"; }
}
$obj = new classA();

echo "\n-- Testing array_fill_keys() function with float --\n";
var_dump( array_fill_keys($array, $float) );

echo "\n-- Testing array_fill_keys() function with null --\n";
var_dump( array_fill_keys($array, $nullVal) );

echo "\n-- Testing array_fill_keys() function with object --\n";
var_dump( array_fill_keys($array, $obj) );

echo "\n-- Testing array_fill_keys() function with boolean --\n";
var_dump( array_fill_keys($array, $bool) );

echo "\n-- Testing array_fill_keys() function with resource --\n";
var_dump( array_fill_keys($array, $fp) );

echo "\n-- Testing array_fill_keys() function with unset var --\n";
var_dump( array_fill_keys($array, $unset_var) );

fclose($fp);
echo "Done";
?>
--EXPECTF--
*** Testing array_fill_keys() : parameter variations ***

-- Testing array_fill_keys() function with float --
array(1) {
  ["one"]=>
  float(2.4)
}

-- Testing array_fill_keys() function with null --
array(1) {
  ["one"]=>
  NULL
}

-- Testing array_fill_keys() function with object --
array(1) {
  ["one"]=>
  object(classA)#%d (0) {
  }
}

-- Testing array_fill_keys() function with boolean --
array(1) {
  ["one"]=>
  bool(false)
}

-- Testing array_fill_keys() function with resource --
array(1) {
  ["one"]=>
  resource(%d) of type (stream)
}

-- Testing array_fill_keys() function with unset var --

Warning: Undefined variable $unset_var in %s on line %d
array(1) {
  ["one"]=>
  NULL
}
Done
