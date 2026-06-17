--TEST--
Verify that #[ZendTestAttributeAddsInterface] adding an interface doesn't leak (manually implement same interface)
--XFAIL--
Currently leaks
--EXTENSIONS--
zend_test
--FILE--
<?php

#[ZendTestAttributeAddsInterface]
class Demo implements _ZendTestInterface {}

var_dump(class_implements(Demo::class));

?>
--EXPECT--
array(1) {
  ["_ZendTestInterface"]=>
  string(18) "_ZendTestInterface"
}
