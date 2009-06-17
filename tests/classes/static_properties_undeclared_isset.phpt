--TEST--
Issetting a non-existent static property
--FILE--
<?php
Class C {}
var_dump(isset(C::$p));
?>
--EXPECTF--
bool(false)