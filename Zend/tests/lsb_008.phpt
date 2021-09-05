--TEST--
ZE2 Late Static Binding class name "static"
--FILE--
<?php
class static {
}
?>
--EXPECTF--
Fatal error: Cannot use 'static' as class name as it is reserved in %s on line 2
