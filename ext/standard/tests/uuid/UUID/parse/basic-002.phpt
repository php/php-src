--TEST--
UUID::parse string representation
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$uuid = UUID::parse('123e4567-e89b-12d3-a456-426655440000');

$p = new ReflectionProperty($uuid, 'bytes');
$p->setAccessible(true);
var_dump($p->getValue($uuid) === "\x12\x3e\x45\x67\xe8\x9b\x12\xd3\xa4\x56\x42\x66\x55\x44\x00\x00");

?>
--EXPECT--
bool(true)
