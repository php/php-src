--TEST--
UUID::__wakeup
--CREDITS--
Richard Fussenegger php@fleshgrinder.com
--FILE--
<?php

$uuid = UUID::v4();
$ser  = serialize($uuid);

var_dump(unserialize($ser) == $uuid);

?>
--EXPECT--
bool(true)
