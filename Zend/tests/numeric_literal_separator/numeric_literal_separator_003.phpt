--TEST--
Invalid use: adjacent underscores
--FILE--
<?php
10__0;
?>
--EXPECTF--
Parse error: syntax error, unexpected identifier "__0" in %s on line %d
