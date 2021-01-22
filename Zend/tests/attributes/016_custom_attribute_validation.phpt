--TEST--
Attribute validation callback of internal attributes.
--SKIPIF--
<?php
if (!extension_loaded('zend_test')) {
    echo "skip requires zend_test extension\n";
}
--FILE--
<?php

#[ZendTestAttribute]
function foo() {
}
?>
--EXPECTF--
Fatal error: Only classes can be marked with #[ZendTestAttribute] in %s
