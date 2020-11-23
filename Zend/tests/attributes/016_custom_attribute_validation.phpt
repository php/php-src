--TEST--
Attribute validation callback of internal attributes.
--SKIPIF--
<?php
if (!extension_loaded('zend-test')) {
    echo "skip requires zend-test extension\n";
}
--FILE--
<?php

#[ZendTestAttribute]
function foo() {
}
?>
--EXPECTF--
Fatal error: Only classes can be marked with #[ZendTestAttribute] in %s
