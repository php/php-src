--TEST--
Traits cannot be readonly
--FILE--
<?php

readonly trait Foo
{
}

?>
--EXPECTF--
Parse error: syntax error, unexpected token "trait", expecting "abstract" or "final" or "readonly" or "class" in %s on line %d
