--TEST--
UUID::$bytes property definition
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$p = new ReflectionProperty(UUID::class, 'bytes');

var_dump($p->isPrivate(), $p->isStatic());

?>
--EXPECT--
bool(true)
bool(false)
