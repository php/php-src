--TEST--
Enums cannot be readonly
--FILE--
<?php

readonly enum Foo
{
}

?>
--EXPECTF--
Parse error: syntax error, unexpected token "enum" in %s on line %d
