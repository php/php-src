--TEST--
GH-17198 (SplFixedArray assertion failure with get_object_vars)
--FILE--
<?php
#[AllowDynamicProperties]
class MySplFixedArray extends SplFixedArray {
}
$array = new MySplFixedArray(2);
$array->{0} = [];
var_dump(get_object_vars($array));
?>
--EXPECT--
array(1) {
  [0]=>
  array(0) {
  }
}
