--TEST--
Interfaces cannot be readonly
--FILE--
<?php

readonly interface Foo
{
}

?>
--EXPECTF--
Parse error: syntax error, unexpected token "interface" in %s on line %d
