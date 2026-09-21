--TEST--
Verify that #[ZendTestAttributeAddsInterface] and manually implementing same interface repeatedly errors
--EXTENSIONS--
zend_test
--FILE--
<?php

#[ZendTestAttributeAddsInterface]
class Demo implements _ZendTestInterface, _ZendTestInterface {}

var_dump(class_implements(Demo::class));

?>
--EXPECTF--
Fatal error: Class Demo cannot implement previously implemented interface _ZendTestInterface in %s on line %d
