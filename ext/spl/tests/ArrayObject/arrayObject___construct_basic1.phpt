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
--EXPECTF--
--> No arguments:
object(ArrayObject)#1 (1) {
  ["storage":"ArrayObject":private]=>
  array(0) {
  }
}
--> Object argument:

Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
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

Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d

Deprecated: ArrayObject::__construct(): Using an object as a backing array for ArrayObject is deprecated, as it allows violating class constraints and invariants in %s on line %d
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
