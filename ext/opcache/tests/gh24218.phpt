--TEST--
GH-24218 (Invalid opcode for count() of a literal array when SCCP is disabled)
--EXTENSIONS--
opcache
--INI--
opcache.enable_cli=1
opcache.optimization_level=0x60
--FILE--
<?php

var_dump(count([1]));
var_dump(count([1, 2, 3]));

?>
--EXPECT--
int(1)
int(3)
