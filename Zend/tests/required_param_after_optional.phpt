--TEST--
Required parameter after optional is deprecated
--FILE--
<?php

function test($testA = null, $testB = null, $testC) {}
function test2(Type $test2A = null, $test2B = null, $test2C) {}
function test3(Type $test3A = null, ?Type2 $test3B = null, $test3C) {}

?>
--EXPECTF--
Deprecated: test(): Optional parameter $testA declared before required parameter $testC is implicitly treated as a required parameter in %s on line %d

Deprecated: test(): Optional parameter $testB declared before required parameter $testC is implicitly treated as a required parameter in %s on line %d

Deprecated: test2(): Implicitly marking parameter $test2A as nullable is deprecated, the explicit nullable type must be used instead in %s on line %d

Deprecated: test2(): Optional parameter $test2B declared before required parameter $test2C is implicitly treated as a required parameter in %s on line %d

Deprecated: test3(): Implicitly marking parameter $test3A as nullable is deprecated, the explicit nullable type must be used instead in %s on line %d

Deprecated: test3(): Optional parameter $test3B declared before required parameter $test3C is implicitly treated as a required parameter in %s on line %d
