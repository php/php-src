--TEST--
scalar type constraint array default value
--DESCRIPTION--
Tests that a scalar parameter of a function cannot have an array as default
value.
--FILE--
<?php

function f(scalar $p = []) {}

?>
--EXPECTF--
Fatal error: Default value for parameters with scalar type can only be bool, float, integer, string, or NULL in %s
