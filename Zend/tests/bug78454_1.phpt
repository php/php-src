--TEST--
Invalid consecutive numeric separators after hex literal
--FILE--
<?php
0x0__F;
--EXPECTF--
Parse error: syntax error, unexpected identifier "__F" in %s on line %d
