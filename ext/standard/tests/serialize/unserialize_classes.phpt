--TEST--
Test unserialize() with second parameter
--FILE--
<?php
class foo {
        public $x = "bar";
}
$z = array(new foo(), 2, "3");
$s = serialize($z);

var_dump(unserialize($s));
var_dump(unserialize($s, ["allowed_classes" => false]));
var_dump(unserialize($s, ["allowed_classes" => true]));
var_dump(unserialize($s, ["allowed_classes" => ["bar"]]));
var_dump(unserialize($s, ["allowed_classes" => ["FOO"]]));
var_dump(unserialize($s, ["allowed_classes" => ["bar", "foO"]]));
?>
--EXPECTF--
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
