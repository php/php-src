--TEST--
Bug #77993 (Wrong parse error for invalid hex literal on Windows)
--FILE--
<?php
0xg10;
--EXPECTF--
Parse error: syntax error, unexpected 'xg10' (T_STRING) in %s on line %d
