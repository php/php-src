--TEST--
Attributes: Compiler Attributes can check for target declarations
--SKIPIF--
<?php
if (!extension_loaded('zend-test')) {
    echo "skip requires zend-test extension\n";
}
--FILE--
<?php

<<ZendTestAttribute>>
function foo() {
}
--EXPECTF--
Fatal error: Only classes can be marked with <<ZendTestAttribute>> in %s
