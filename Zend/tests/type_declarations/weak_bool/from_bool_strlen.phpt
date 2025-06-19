--TEST--
Check deprecation from bool type in weak mode for specialized strlen() opcode
--FILE--
<?php

var_dump(strlen(false));
var_dump(strlen(true));

?>
--EXPECTF--
Deprecated: strlen(): Passing bool to parameter #1 ($string) of type string is deprecated in %s on line %d
int(0)

Deprecated: strlen(): Passing bool to parameter #1 ($string) of type string is deprecated in %s on line %d
int(1)
