--TEST--
Invalid use: underscore next to 0b
--FILE--
<?php
0b_0101;
--EXPECTF--
Parse error: syntax error, unexpected 'b_0101' (T_STRING) in %s on line %d
