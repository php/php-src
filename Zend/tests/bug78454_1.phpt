--TEST--
Invalid consecutive numeric separators after hex literal
--FILE--
<?php
0x0__F;
--EXPECTF--
Parse error: syntax error, unexpected '__F' (T_STRING) in %s on line %d
