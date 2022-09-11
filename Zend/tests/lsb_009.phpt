--TEST--
ZE2 Late Static Binding interface name "static"
--FILE--
<?php
interface static {
}
?>
--EXPECTF--
Fatal error: Cannot use 'static' as class name as it is reserved in %s on line 2
