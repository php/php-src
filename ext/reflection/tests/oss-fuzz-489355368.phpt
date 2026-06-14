--TEST--
OSS-Fuzz #489355368: Incorrect assumption Z_PTR_P assumption
--FILE--
<?php

#[AllowDynamicProperties]
class A {}

$obj = new A;
$obj->prop = 0;

$rp = new ReflectionProperty($obj, 'prop');
var_dump($rp->isReadable(null, $obj));

?>
--EXPECT--
bool(true)
