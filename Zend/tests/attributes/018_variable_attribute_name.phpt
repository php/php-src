--TEST--
Attribute name cannot be a variable
--FILE--
<?php

<<$x>>
class A {}

?>
--EXPECTF--
Parse error: syntax error, unexpected '$x' (T_VARIABLE) in %s on line %d
