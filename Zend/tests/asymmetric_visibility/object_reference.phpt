--TEST--
Attempting to obtain reference of object of private(set) object returns a copy instead
--FILE--
<?php

class Foo {
    public private(set) Bar $bar;

    public function __construct() {
        $this->bar = new Bar();
    }
}

class Bar {}

function test() {
    $foo = new Foo();
    $bar = &$foo->bar;
    var_dump($foo);
}

test();
// Test zend_fetch_property_address with warmed cache
test();

?>
--EXPECT--
object(Foo)#1 (1) {
  ["bar"]=>
  object(Bar)#2 (0) {
  }
}
object(Foo)#2 (1) {
  ["bar"]=>
  object(Bar)#1 (0) {
  }
}
