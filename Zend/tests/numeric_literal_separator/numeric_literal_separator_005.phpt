--TEST--
Invalid use: underscore right of period
--FILE--
<?php
100._0;
?>
--EXPECTF--
Parse error: syntax error, unexpected identifier "_0" in %s on line %d
