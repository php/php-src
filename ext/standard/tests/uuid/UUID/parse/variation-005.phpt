--TEST--
UUID::parse accepts URN even with leading and trailing whitespace/braces and extraneous hyphens everywhere
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;

$uuid = UUID::parse(" \t ---- { urn:uuid:----0123-4567-89ab-cdef-0123-4567-89ab-cdef---- } ---- \t ");

$p = new ReflectionProperty($uuid, 'bytes');
$p->setAccessible(true);
var_dump($p->getValue($uuid) === "\x01\x23\x45\x67\x89\xab\xcd\xef\x01\x23\x45\x67\x89\xab\xcd\xef");

?>
--EXPECT--
bool(true)
