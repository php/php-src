--TEST--
Tests that a trait cannot be declared static
--FILE--
<?php

static trait T {}
?>
--EXPECTF--
Parse error: syntax error, unexpected token "trait" in %s on line %d
