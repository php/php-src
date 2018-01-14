--TEST--
Single comma in function calls is not allowed
--FILE--
<?php
foo(,);
?>
--EXPECTF--
Parse error: syntax error, unexpected ',' in %s on line %d
