--TEST--
Bug #78833 (Integer overflow in pack causes out-of-bound access)
--FILE--
<?php
var_dump(pack("E2E2147483647H*", 0x0, 0x0, 0x0));
?>
--EXPECTF--
Warning: pack(): Type E: too few arguments in %s on line %d
bool(false)
