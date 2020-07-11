--TEST--
Test array_fill_keys() function : variation of parameter
--FILE--
<?php
echo "*** Testing array_fill_keys() : parameter variations ***\n";

$nullVal = null;
$simpleStr = "simple";
$fp = fopen(__FILE__, "r");
$emptyArr = array();
$bool = false;
$float = 2.4;

class classA {
  public function __toString() { return "Class A object"; }
}
$obj = new classA();


echo "\n-- Testing array_fill_keys() function with empty arguments --\n";
var_dump( array_fill_keys($emptyArr, $nullVal) );

echo "\n-- Testing array_fill_keys() function with keyed array --\n";
$keyedArray = array("two" => 2, "strk1" => "strv1", 4, $simpleStr);
var_dump( array_fill_keys($keyedArray, $simpleStr) );

echo "\n-- Testing array_fill_keys() function with mixed array --\n";
$mixedArray = array($fp, $obj, $simpleStr, $emptyArr, 2, $bool, $float);
var_dump( array_fill_keys($mixedArray, $simpleStr) );

fclose($fp);
echo "Done";
?>
--EXPECTF--
*** Testing array_fill_keys() : parameter variations ***

-- Testing array_fill_keys() function with empty arguments --
array(0) {
}

-- Testing array_fill_keys() function with keyed array --
array(4) {
  [2]=>
  string(6) "simple"
  ["strv1"]=>
  string(6) "simple"
  [4]=>
  string(6) "simple"
  ["simple"]=>
  string(6) "simple"
}

-- Testing array_fill_keys() function with mixed array --

Warning: Array to string conversion in %s on line %d
array(7) {
  ["Resource id #%d"]=>
  string(6) "simple"
  ["Class A object"]=>
  string(6) "simple"
  ["simple"]=>
  string(6) "simple"
  ["Array"]=>
  string(6) "simple"
  [2]=>
  string(6) "simple"
  [""]=>
  string(6) "simple"
  ["2.4"]=>
  string(6) "simple"
}
Done
