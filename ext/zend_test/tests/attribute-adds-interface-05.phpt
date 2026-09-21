--TEST--
Verify that #[ZendTestAttributeAddsInterface] and manually implementing a different interface repeatedly errors
--EXTENSIONS--
zend_test
--FILE--
<?php

interface MyInterface {}

#[ZendTestAttributeAddsInterface]
class Demo implements MyInterface, MyInterface {}

var_dump(class_implements(Demo::class));

?>
--EXPECTF--
Fatal error: Class Demo cannot implement previously implemented interface MyInterface in %s on line %d
