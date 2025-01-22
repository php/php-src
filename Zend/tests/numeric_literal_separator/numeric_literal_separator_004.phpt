--TEST--
Invalid use: underscore left of period
--FILE--
<?php
100_.0;
?>
--EXPECTF--
Parse error: syntax error, unexpected identifier "_" in %s on line %d
