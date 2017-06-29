--TEST--
UUID::fromBinary random data
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;

$uuid = UUID::fromBinary('                ');
$p    = new ReflectionProperty($uuid, 'bytes');
$p->setAccessible(true);
var_dump($p->getValue($uuid) === '                ');

?>
--EXPECT--
bool(true)
