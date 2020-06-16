--TEST--
Attribute name cannot be a variable
--FILE--
<?php

<<$x>>
class A {}

?>
--EXPECTF--
Parse error: syntax error, unexpected '$x' (T_VARIABLE), expecting identifier (T_STRING) or static (T_STATIC) or namespace (T_NAMESPACE) or \\ (T_NS_SEPARATOR) in %s on line %d
