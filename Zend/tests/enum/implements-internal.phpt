--TEST--
Enum implements (internal enum)
--EXTENSIONS--
zend_test
--FILE--
<?php

var_dump(ZendTestUnitEnum::Foo instanceof _ZendTestInterface);
var_dump(ZendTestEnumWithInterface::Foo instanceof _ZendTestInterface);

?>
--EXPECT--
bool(false)
bool(true)
