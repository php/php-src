--TEST--
UUID::__wakeup
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

use PHP\Std\UUID;

$uuid = UUID::fromBinary("\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1\1");

var_dump($uuid == unserialize(serialize($uuid)));

?>
--EXPECT--
bool(true)
