--TEST--
Bug #72785: allowed_classes only applies to outermost unserialize()
--FILE--
<?php

// Forbidden class
class A {}

$s = serialize(new ArrayObject(array(new A())));
var_dump(unserialize($s, ['allowed_classes' => ['ArrayObject']]));

?>
--EXPECT--
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(1) {
    [0]=>
    object(__PHP_Incomplete_Class)#2 (1) {
      ["__PHP_Incomplete_Class_Name"]=>
      string(1) "A"
    }
  }
}
