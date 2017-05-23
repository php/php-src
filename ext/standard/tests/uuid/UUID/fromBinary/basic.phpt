--TEST--
UUID::fromBinary random data
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$bytes = random_bytes(16);
$uuid  = UUID::fromBinary($bytes);

$p = new ReflectionProperty($uuid, 'binary');
$p->setAccessible(true);
var_dump($p->getValue($uuid) === $bytes);

?>
--EXPECT--
bool(true)
