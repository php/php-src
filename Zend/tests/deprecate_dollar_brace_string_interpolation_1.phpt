--TEST--
"${}" emits a deprecation
--FILE--
<?php

$foo = 'bar';
var_dump("${foo}");

?>
--EXPECTF--
Deprecated: Using ${var} in strings is deprecated, use {$var} instead in %s on line %d
string(3) "bar"
