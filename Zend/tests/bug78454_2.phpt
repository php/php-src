--TEST--
Invalid consecutive numeric separators after binary literal
--FILE--
<?php
0b0__1
--EXPECTF--
Parse error: syntax error, unexpected '__1' (T_STRING) in %s on line %d
