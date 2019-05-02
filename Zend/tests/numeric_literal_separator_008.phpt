--TEST--
Invalid use: underscore left of e
--FILE--
<?php
1_e2;
--EXPECTF--
Parse error: syntax error, unexpected '_e2' (T_STRING) in %s on line %d
