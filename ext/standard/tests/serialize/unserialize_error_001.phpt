--TEST--
Test unserialize() with non-bool/array allowed_classes
--FILE--
<?php
class foo {
        public $x = "bar";
}
$z = array(new foo(), 2, "3");
$s = serialize($z);

var_dump(unserialize($s, ["allowed_classes" => null]));
var_dump(unserialize($s, ["allowed_classes" => 0]));
var_dump(unserialize($s, ["allowed_classes" => 1]));

--EXPECTF--
array(3) {
  [0]=>
  object(__PHP_Incomplete_Class)#%d (2) {
    ["__PHP_Incomplete_Class_Name"]=>
    string(3) "foo"
    ["x"]=>
    string(3) "bar"
  }
  [1]=>
  int(2)
  [2]=>
  string(1) "3"
}
array(3) {
  [0]=>
  object(__PHP_Incomplete_Class)#%d (2) {
    ["__PHP_Incomplete_Class_Name"]=>
    string(3) "foo"
    ["x"]=>
    string(3) "bar"
  }
  [1]=>
  int(2)
  [2]=>
  string(1) "3"
}
array(3) {
  [0]=>
  object(foo)#%d (1) {
    ["x"]=>
    string(3) "bar"
  }
  [1]=>
  int(2)
  [2]=>
  string(1) "3"
}
