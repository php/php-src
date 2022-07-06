--TEST--
Invalid use: underscore right of e
--FILE--
<?php
1e_2;
?>
--EXPECTF--
Parse error: syntax error, unexpected identifier "e_2" in %s on line %d
