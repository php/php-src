--TEST--
Accessors are not class methods
--FILE--
<?php

class Foo {
    public $testProperty { get {} set {} }
    public function testMethod() {}
}

var_dump(get_class_methods('Foo'));

?>
--EXPECT--
array(1) {
  [0]=>
  string(10) "testMethod"
}
