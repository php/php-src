--TEST--
Invalid use: underscore next to 0b
--FILE--
<?php
0b_0101;
?>
--EXPECTF--
Parse error: syntax error, unexpected identifier "b_0101" in %s on line %d
