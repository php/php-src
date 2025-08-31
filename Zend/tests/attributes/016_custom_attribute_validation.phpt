--TEST--
Attribute validation callback of internal attributes.
--EXTENSIONS--
zend_test
--FILE--
<?php

#[ZendTestAttribute]
function foo() {
}
?>
--EXPECTF--
Fatal error: Only classes can be marked with #[ZendTestAttribute] in %s
