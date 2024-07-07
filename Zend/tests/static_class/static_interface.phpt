--TEST--
Tests that an interface cannot be declared static
--FILE--
<?php

static interface I {}
?>
--EXPECTF--
Parse error: syntax error, unexpected token "interface" in %s on line %d
