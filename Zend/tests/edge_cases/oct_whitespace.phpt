--TEST--
Octal literal followed by whitespace and another number
--FILE--
<?php
var_dump(0o0 2);
?>
--EXPECTF--
Parse error: syntax error, unexpected integer "2", expecting ")" in %s on line %d
