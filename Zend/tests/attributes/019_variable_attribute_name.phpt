--TEST--
Attribute name cannot be a variable
--FILE--
<?php

<<$x>>
class A {}

?>
--EXPECTF--
Parse error: syntax error, unexpected variable "$x", expecting identifier or "static" or "namespace" or "\" in %s on line %d
