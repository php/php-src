--TEST--
Enums cannot be readonly
--FILE--
<?php

readonly enum Foo
{
}

?>
--EXPECTF--
Parse error: syntax error, unexpected token "enum", expecting "abstract" or "final" or "readonly" or "class" in %s on line %d
