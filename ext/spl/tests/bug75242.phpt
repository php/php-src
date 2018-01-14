--TEST--
Bug #75242: RecursiveArrayIterator doesn't have constants from parent class
--FILE--
<?php

class Foo extends ArrayIterator { }

$r = new ReflectionClass(Foo::class);
var_dump($r->getConstants());
$r = new ReflectionClass(ArrayIterator::class);
var_dump($r->getConstants());
$r = new ReflectionClass(RecursiveArrayIterator::class);
var_dump($r->getConstants());

?>
--EXPECT--
array(2) {
  ["STD_PROP_LIST"]=>
  int(1)
  ["ARRAY_AS_PROPS"]=>
  int(2)
}
array(2) {
  ["STD_PROP_LIST"]=>
  int(1)
  ["ARRAY_AS_PROPS"]=>
  int(2)
}
array(3) {
  ["STD_PROP_LIST"]=>
  int(1)
  ["ARRAY_AS_PROPS"]=>
  int(2)
  ["CHILD_ARRAYS_ONLY"]=>
  int(4)
}
