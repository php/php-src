--TEST--
Invalid use: underscore right of period
--FILE--
<?php
100._0;
--EXPECTF--
Parse error: syntax error, unexpected '_0' (T_STRING) in %s on line %d
