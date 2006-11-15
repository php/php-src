--TEST--
compact() tests
--FILE--
<?php

$var1 = "test";
$var2 = "one more";
$var3 = "and the last one";
$_ = "a";

var_dump(compact("var1", "var2", "var3"));
var_dump(compact(""));
var_dump(compact("-1"));
var_dump(compact("."));
var_dump(compact("_"));
var_dump(compact("var3", "var4"));
var_dump(compact(array("var2", "var3")));
var_dump(compact(array(array("var1"), array("var1"))));

$a = array(2.0, -5);
var_dump(compact($a));

echo "Done\n";
?>
--EXPECTF--
array(3) {
  ["var1"]=>
  string(4) "test"
  ["var2"]=>
  string(8) "one more"
  ["var3"]=>
  string(16) "and the last one"
}
array(0) {
}
array(0) {
}
array(0) {
}
array(1) {
  ["_"]=>
  string(1) "a"
}
array(1) {
  ["var3"]=>
  string(16) "and the last one"
}
array(2) {
  ["var2"]=>
  string(8) "one more"
  ["var3"]=>
  string(16) "and the last one"
}
array(1) {
  ["var1"]=>
  string(4) "test"
}
array(0) {
}
Done
