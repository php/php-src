--TEST--
Verify that #[ZendTestAttributeAddsInterface] adding an interface doesn't leak (manually implement different interface)
--XFAIL--
Currently leaks and overwrites the interface added by the attribute
--EXTENSIONS--
zend_test
--FILE--
<?php

interface MyInterface {}

#[ZendTestAttributeAddsInterface]
class Demo implements MyInterface {}

var_dump(class_implements(Demo::class));

?>
--EXPECT--
array(2) {
  ["_ZendTestInterface"]=>
  string(18) "_ZendTestInterface"
  ["MyInterface"]=>
  string(11) "MyInterface"
}
