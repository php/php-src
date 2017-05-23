--TEST--
UUID::$binary property definition
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$p = new ReflectionProperty(UUID::class, 'binary');

var_dump($p->isPrivate(), $p->isStatic());

?>
--EXPECT--
bool(true)
bool(false)
