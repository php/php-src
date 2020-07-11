--TEST--
Invalid use: adjacent underscores
--FILE--
<?php
10__0;
--EXPECTF--
Parse error: syntax error, unexpected '__0' (T_STRING) in %s on line %d
