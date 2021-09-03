--TEST--
ZE2 Late Static Binding class name "static"
--FILE--
<?php
class static {
}
?>
--EXPECTF--
Parse error: %s error,%sexpecting %s in %s on line %d
