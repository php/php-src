--TEST--
Test invalid syntax of traits with the immutable modifier
--FILE--
<?php

immutable trait Foo
{
}

?>
--EXPECTF--
Parse error: syntax error, unexpected 'trait' (T_TRAIT), expecting abstract (T_ABSTRACT) or final (T_FINAL) or immutable (T_IMMUTABLE) or class (T_CLASS) in %s on line %d
