--TEST--
ZE2 Late Static Binding interface name "static"
--FILE--
<?php
interface static {
}
?>
--EXPECTF--
Parse error: %s error,%sexpecting %s in %s on line %d
