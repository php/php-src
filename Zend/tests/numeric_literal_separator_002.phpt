--TEST--
Invalid use: trailing underscore
--FILE--
<?php
100_;
--EXPECTF--
Parse error: syntax error, unexpected '_' (T_STRING) in %s on line %d
