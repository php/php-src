--TEST--
SplFixedArray - get_properties_for handlers
--FILE--
<?php
#[AllowDynamicProperties]
class MySplFixedArray extends SplFixedArray {
    public $x;
    public int $y;
}
class X {}
class Y {}
$array = new MySplFixedArray(2);
var_dump(get_mangled_object_vars($array));
$array[0] = new stdClass();
$array[1] = new Y();
$array->x = new SplFixedArray();
$array->{0} = new X();
var_dump($array);
// As of php 8.3, get_mangled_object_vars only contains object properties (dynamic properties and declared subclass properties)
//                (Array elements in the SplFixedArray are deliberately excluded)
// Before php 8.3, this would have array elements get removed in some cases but not others.
var_dump(get_mangled_object_vars($array));
echo "cast to array\n";
var_dump((array)$array); // Adds the values from the underlying array, then the declared/dynamic object properties
echo json_encode($array), "\n"; // From JsonSerializable::serialize()
$ser = serialize($array);
echo "$ser\n";
// NOTE: The unserialize behavior for the property that is the string '0' is just because unserialize()
// is coercing '0' to a string before calling SplFixedArray::__unserialize.
//
// Typical code would not use 0 as a property name, this test is just testing edge cases have proper reference counting and so on.
var_dump(unserialize($ser));
?>
--EXPECT--
array(1) {
  ["x"]=>
  NULL
}
object(MySplFixedArray)#1 (4) {
  [0]=>
  object(stdClass)#2 (0) {
  }
  [1]=>
  object(Y)#3 (0) {
  }
  ["x"]=>
  object(SplFixedArray)#4 (0) {
  }
  ["0"]=>
  object(X)#5 (0) {
  }
}
array(2) {
  ["x"]=>
  object(SplFixedArray)#4 (0) {
  }
  [0]=>
  object(X)#5 (0) {
  }
}
cast to array
array(3) {
  [0]=>
  object(X)#5 (0) {
  }
  [1]=>
  object(Y)#3 (0) {
  }
  ["x"]=>
  object(SplFixedArray)#4 (0) {
  }
}
[{},{}]
O:15:"MySplFixedArray":4:{i:0;O:8:"stdClass":0:{}i:1;O:1:"Y":0:{}s:1:"x";O:13:"SplFixedArray":0:{}s:1:"0";O:1:"X":0:{}}
object(MySplFixedArray)#6 (3) {
  [0]=>
  object(X)#10 (0) {
  }
  [1]=>
  object(Y)#8 (0) {
  }
  ["x"]=>
  object(SplFixedArray)#9 (0) {
  }
}
