--TEST--
UUIDParsingException position property definition.
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$p = new ReflectionProperty(UUIDParsingException::class, 'position');

var_dump($p->isPrivate(), $p->isStatic());

?>
--EXPECT--
bool(true)
bool(false)
