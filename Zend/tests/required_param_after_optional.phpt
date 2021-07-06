--TEST--
Required parameter after optional is deprecated
--FILE--
<?php

function test($testA = null, $testB = null, $testC) {}
function test2(Type $test2A = null, $test2B = null, $test2C) {}
function test3(Type $test3A = null, ?Type2 $test3B = null, $test3C) {}

?>
--EXPECTF--
Deprecated: Optional parameter $testA declared before required parameter $testC is implicitly treated as a required parameter in %s on line %d

Deprecated: Optional parameter $testB declared before required parameter $testC is implicitly treated as a required parameter in %s on line %d

Deprecated: Optional parameter $test2B declared before required parameter $test2C is implicitly treated as a required parameter in %s on line %d

Deprecated: Optional parameter $test3B declared before required parameter $test3C is implicitly treated as a required parameter in %s on line %d
