--TEST--
Traits cannot be readonly
--FILE--
<?php

readonly trait Foo
{
}

?>
--EXPECTF--
Parse error: syntax error, unexpected token "trait" in %s on line %d
