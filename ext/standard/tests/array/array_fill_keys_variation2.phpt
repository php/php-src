--TEST--
Test array_fill_keys() function : variation of parameter
--FILE--
<?php
/* Testing with reference types for the arguments */

echo "*** Testing array_fill_keys() : parameter variations ***\n";

$nullVal = null;
$simpleStr = "simple";
$refString = &$simpleStr;
$fp = fopen(__FILE__, "r");
$emptyArr = array();
$bool = false;
$float = 2.4;

class classA {
  public function __toString() { return "Class A object"; }
}
$obj = new classA();


echo "\n-- Testing array_fill_keys() function with reference value --\n";
$keyedArray = array("one", "two");
var_dump(array_fill_keys($keyedArray, $refString));

echo "\n-- Testing array_fill_keys() function with reference keys --\n";
$refKeys = array("one", &$simpleStr);
$res = array_fill_keys($refKeys, $simpleStr);
var_dump($res);
$simpleStr = "bob";
var_dump($res);


echo "\n-- Testing array_fill_keys() function with reference array input --\n";
$newArray = array("one", "two");
$refArray = &$newArray;
var_dump(array_fill_keys($refArray, $simpleStr));

fclose($fp);
echo "Done";
?>
--EXPECT--
*** Testing array_fill_keys() : parameter variations ***

-- Testing array_fill_keys() function with reference value --
array(2) {
  ["one"]=>
  string(6) "simple"
  ["two"]=>
  string(6) "simple"
}

-- Testing array_fill_keys() function with reference keys --
array(2) {
  ["one"]=>
  string(6) "simple"
  ["simple"]=>
  string(6) "simple"
}
array(2) {
  ["one"]=>
  string(6) "simple"
  ["simple"]=>
  string(6) "simple"
}

-- Testing array_fill_keys() function with reference array input --
array(2) {
  ["one"]=>
  string(3) "bob"
  ["two"]=>
  string(3) "bob"
}
Done
