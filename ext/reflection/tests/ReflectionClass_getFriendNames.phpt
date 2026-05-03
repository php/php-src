--TEST--
ReflectionClass::getFriendNames() method
--FILE--
<?php

use Demo\Qux;

class Foo {
	friend Bar;
	friend Demo\Bar;
	friend Baz;
	friend Qux;
}

$rc = new ReflectionClass(Foo::class);
var_dump($rc->getFriendNames());

$rc = new ReflectionClass(ReflectionClass::class);
var_dump($rc->getFriendNames());

?>
--EXPECT--
array(4) {
  [0]=>
  string(3) "Bar"
  [1]=>
  string(8) "Demo\Bar"
  [2]=>
  string(3) "Baz"
  [3]=>
  string(8) "Demo\Qux"
}
array(0) {
}
