--TEST--
Interfaces cannot be readonly
--FILE--
<?php

readonly interface Foo
{
}

?>
--EXPECTF--
Parse error: syntax error, unexpected token "interface", expecting "abstract" or "final" or "readonly" or "class" in %s on line %d
