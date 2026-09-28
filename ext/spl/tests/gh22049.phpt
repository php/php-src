--TEST--
GH-22049: The unserialize function with ArrayIterator leads to crash when object cloned
--FILE--
<?php

$payload = 'O:13:"ArrayIterator":3:{s:1:"0";i:0;s:1:"1";a:0:{}s:1:"2";a:2:{i:0;i:42;i:1;i:99;}}';
$obj = @unserialize($payload);
$clone = clone $obj;
var_dump(get_mangled_object_vars($clone));

$it = new ArrayIterator();
@$it->{0} = 'first';
@$it->{1} = 'second';
$serialized = serialize($it);
$unserialized = @unserialize($serialized);
$clonedIt = clone $unserialized;
var_dump(get_mangled_object_vars($clonedIt));

$obj2 = new ArrayObject();
@$obj2->{0} = 'ao_first';
@$obj2->{1} = 'ao_second';
$serialized2 = serialize($obj2);
$unserialized2 = @unserialize($serialized2);
$clonedObj2 = clone $unserialized2;
var_dump(get_mangled_object_vars($clonedObj2));

echo "DONE\n";
?>
--EXPECT--
array(2) {
  [0]=>
  int(42)
  [1]=>
  int(99)
}
array(2) {
  [0]=>
  string(5) "first"
  [1]=>
  string(6) "second"
}
array(2) {
  [0]=>
  string(8) "ao_first"
  [1]=>
  string(9) "ao_second"
}
DONE
