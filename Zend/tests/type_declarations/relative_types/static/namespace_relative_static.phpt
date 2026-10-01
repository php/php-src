--TEST--
namespace\static is not a valid type declaration
--DESCRIPTION--
OSS-Fuzz: https://issues.oss-fuzz.com/issues/532353396
--FILE--
<?php

function test($i): namespace\static {}

?>
--EXPECTF--
Fatal error: Type declaration 'static' must be unqualified in %s on line %d
