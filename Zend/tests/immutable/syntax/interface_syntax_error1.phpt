--TEST--
Test invalid syntax of traits with the immutable modifier
--FILE--
<?php

immutable interface Foo
{
}

?>
--EXPECTF--
Parse error: syntax error, unexpected 'interface' (T_INTERFACE), expecting abstract (T_ABSTRACT) or final (T_FINAL) or immutable (T_IMMUTABLE) or class (T_CLASS) in %s on line %d
