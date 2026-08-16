--TEST--
Verify that #[ZendTestAttributeAddsInterface] adding an interface doesn't leak (no manual implements)
--EXTENSIONS--
zend_test
--FILE--
<?php

#[ZendTestAttributeAddsInterface]
class Demo {}

var_dump(class_implements(Demo::class));

?>
--EXPECT--
array(1) {
  ["_ZendTestInterface"]=>
  string(18) "_ZendTestInterface"
}
