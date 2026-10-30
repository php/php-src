--TEST--
GH-10907 (Unable to serialize processed SplFixedArrays in PHP 8.2.4)
--FILE--
<?php
echo "Test without rebuilding properties\n";
$array = new SplFixedArray(2);
$array[0] = "test value 1";
$array[1] = "test value 2";
var_dump(serialize($array));
var_dump(unserialize(serialize($array)));
var_dump($array);

echo "=================\n";

echo "Test with rebuilding properties\n";
$array = new SplFixedArray(2);
$array[0] = "test value 1";
$array[1] = "test value 2";
var_dump($array); // Rebuilds properties
var_dump(serialize($array));
var_dump(unserialize(serialize($array)));
var_dump($array);

echo "=================\n";

echo "Test with partially rebuilding properties\n";
$array = new SplFixedArray(3);
$array[0] = "test value 1";
var_dump($array); // Rebuilds properties
$array[1] = "test value 2";
var_dump(serialize($array));
var_dump(unserialize(serialize($array)));
var_dump($array);

echo "=================\n";

echo "Test with adding members\n";
#[AllowDynamicProperties]
class MySplFixedArray extends SplFixedArray {
    public string $my_string = "my_string_value";
}
$array = new MySplFixedArray(3);
$array->my_dynamic_property = "my_dynamic_property_value";
$array[0] = "test value 1";
$array[1] = "test value 2";
var_dump(serialize($array));
var_dump(unserialize(serialize($array)));
var_dump($array);
?>
--EXPECT--
Test without rebuilding properties
string(73) "O:13:"SplFixedArray":2:{i:0;s:12:"test value 1";i:1;s:12:"test value 2";}"
object(SplFixedArray)#2 (2) {
  [0]=>
  string(12) "test value 1"
  [1]=>
  string(12) "test value 2"
}
object(SplFixedArray)#1 (2) {
  [0]=>
  string(12) "test value 1"
  [1]=>
  string(12) "test value 2"
}
=================
Test with rebuilding properties
object(SplFixedArray)#2 (2) {
  [0]=>
  string(12) "test value 1"
  [1]=>
  string(12) "test value 2"
}
string(73) "O:13:"SplFixedArray":2:{i:0;s:12:"test value 1";i:1;s:12:"test value 2";}"
object(SplFixedArray)#1 (2) {
  [0]=>
  string(12) "test value 1"
  [1]=>
  string(12) "test value 2"
}
object(SplFixedArray)#2 (2) {
  [0]=>
  string(12) "test value 1"
  [1]=>
  string(12) "test value 2"
}
=================
Test with partially rebuilding properties
object(SplFixedArray)#1 (3) {
  [0]=>
  string(12) "test value 1"
  [1]=>
  NULL
  [2]=>
  NULL
}
string(79) "O:13:"SplFixedArray":3:{i:0;s:12:"test value 1";i:1;s:12:"test value 2";i:2;N;}"
object(SplFixedArray)#2 (3) {
  [0]=>
  string(12) "test value 1"
  [1]=>
  string(12) "test value 2"
  [2]=>
  NULL
}
object(SplFixedArray)#1 (3) {
  [0]=>
  string(12) "test value 1"
  [1]=>
  string(12) "test value 2"
  [2]=>
  NULL
}
=================
Test with adding members
string(180) "O:15:"MySplFixedArray":5:{i:0;s:12:"test value 1";i:1;s:12:"test value 2";i:2;N;s:9:"my_string";s:15:"my_string_value";s:19:"my_dynamic_property";s:25:"my_dynamic_property_value";}"
object(MySplFixedArray)#1 (5) {
  [0]=>
  string(12) "test value 1"
  [1]=>
  string(12) "test value 2"
  [2]=>
  NULL
  ["my_string"]=>
  string(15) "my_string_value"
  ["my_dynamic_property"]=>
  string(25) "my_dynamic_property_value"
}
object(MySplFixedArray)#2 (5) {
  [0]=>
  string(12) "test value 1"
  [1]=>
  string(12) "test value 2"
  [2]=>
  NULL
  ["my_string"]=>
  string(15) "my_string_value"
  ["my_dynamic_property"]=>
  string(25) "my_dynamic_property_value"
}
