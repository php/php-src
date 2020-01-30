--TEST--
Test invalid syntax of immutable anonymous classes
--FILE--
<?php

new immutable class() {
};

?>
--EXPECTF--
Parse error: syntax error, unexpected 'immutable' (T_IMMUTABLE) in %s on line %d
