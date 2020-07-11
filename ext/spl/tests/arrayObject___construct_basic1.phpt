--TEST--
SPL: ArrayObject::__construct basic usage.
--FILE--
<?php
echo "--> No arguments:\n";
var_dump(new ArrayObject());

echo "--> Object argument:\n";
$a = new stdClass;
$a->p = 'hello';
var_dump(new ArrayObject($a));

echo "--> Array argument:\n";
var_dump(new ArrayObject(array('key1' => 'val1')));

echo "--> Nested ArrayObject argument:\n";
var_dump(new ArrayObject(new ArrayObject($a)));
?>
--EXPECT--
--> No arguments:
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
--> Object argument:
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  object(stdClass)#1 (1) {
    ["p"]=>
    string(5) "hello"
  }
}
--> Array argument:
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  array(1) {
    ["key1"]=>
    string(4) "val1"
  }
}
--> Nested ArrayObject argument:
object(ArrayObject)#2 (1) {
  ["storage":"ArrayObject":private]=>
  object(ArrayObject)#3 (1) {
    ["storage":"ArrayObject":private]=>
    object(stdClass)#1 (1) {
      ["p"]=>
      string(5) "hello"
    }
  }
}
