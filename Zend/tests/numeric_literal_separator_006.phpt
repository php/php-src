--TEST--
Invalid use: underscore next to 0x
--FILE--
<?php
0x_0123;
--EXPECTF--
Parse error: syntax error, unexpected 'x_0123' (T_STRING) in %s on line %d
