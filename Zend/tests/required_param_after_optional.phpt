--TEST--
Required parameter after optional is deprecated
--FILE--
<?php

function test($testA = null, $testB = null, $testC) {}
function test2(Type $test2A = null, $test2B = null, $test2C) {}
function test3(Type $test3A = null, Type2 $test3B = null, $test3C) {}

?>
--EXPECTF--
Deprecated: Required parameter $testC follows optional parameter $testA in %s on line %d

Deprecated: Required parameter $test2C follows optional parameter $test2B in %s on line %d
