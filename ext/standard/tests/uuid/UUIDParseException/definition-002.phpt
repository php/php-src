--TEST--
UUIDParseException input property definition.
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$p = new ReflectionProperty(UUIDParseException::class, 'input');

var_dump($p->isPrivate(), $p->isStatic());

?>
--EXPECT--
bool(true)
bool(false)
