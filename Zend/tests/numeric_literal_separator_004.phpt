--TEST--
Invalid use: underscore left of period
--FILE--
<?php
100_.0;
--EXPECTF--
Parse error: syntax error, unexpected '_' (T_STRING) in %s on line %d
