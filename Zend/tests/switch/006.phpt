--TEST--
Switch expression with no cases
--FILE--
<?php

$x = switch (true) {};

?>
--EXPECTF--
Parse error: syntax error, unexpected '}' in %s on line %d
