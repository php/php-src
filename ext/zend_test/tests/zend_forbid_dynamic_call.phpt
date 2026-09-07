--TEST--
Zend: Test zend_forbid_dynamic_call() for methods
--EXTENSIONS--
zend_test
--FILE--
<?php

$object = new ZendTestForbidDynamicCall();
$object->call();
ZendTestForbidDynamicCall::callStatic();

try {
    $call = [$object, 'call'];
    $call();
} catch (Error $error) {
    echo $error::class, ': ', $error->getMessage(), "\n";
}

try {
    $callStatic = [ZendTestForbidDynamicCall::class, 'callStatic'];
    $callStatic();
} catch (Error $error) {
    echo $error::class, ': ', $error->getMessage(), "\n";
}

?>
--EXPECT--
Error: Cannot call ZendTestForbidDynamicCall::call() dynamically
Error: Cannot call ZendTestForbidDynamicCall::callStatic() dynamically
