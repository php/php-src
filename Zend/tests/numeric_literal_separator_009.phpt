--TEST--
Invalid use: underscore right of e
--FILE--
<?php
1e_2;
--EXPECTF--
Parse error: syntax error, unexpected 'e_2' (T_STRING) in %s on line %d
