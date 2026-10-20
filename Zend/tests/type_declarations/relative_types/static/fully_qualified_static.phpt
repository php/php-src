--TEST--
Fully qualified (leading backslash) static type names must fail
--DESCRIPTION--
OSS-Fuzz: https://issues.oss-fuzz.com/issues/532353396
--FILE--
<?php

function test($i): \static {}

?>
--EXPECTF--
Fatal error: Type declaration 'static' must be unqualified in %s on line %d
