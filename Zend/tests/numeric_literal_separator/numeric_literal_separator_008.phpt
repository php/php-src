--TEST--
Invalid use: underscore left of e
--FILE--
<?php
1_e2;
?>
--EXPECTF--
Parse error: syntax error, unexpected identifier "_e2" in %s on line %d
