--TEST--
UUID::fromBinary random data
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$uuid  = UUID::fromBinary('                ');
$p = new ReflectionProperty($uuid, 'binary');
$p->setAccessible(true);
var_dump($p->getValue($uuid) === '                ');

?>
--EXPECT--
bool(true)
