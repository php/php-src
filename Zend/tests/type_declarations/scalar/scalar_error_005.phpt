--TEST--
invalid array scalar type constraint parameter default value
--FILE--
<?php

function f(scalar $p = []) {}

?>
--EXPECTF--
Fatal error: Default value for parameters with scalar type can only be bool, float, integer, string, or NULL in %s
