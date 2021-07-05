--TEST--
Parse error for & not followed by var or vararg
--FILE--
<?php
+&
?>
--EXPECTF--
Parse error: syntax error, unexpected token "&" in %s on line %d
