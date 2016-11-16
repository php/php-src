--TEST--
Bug #73532 (Null pointer dereference in mb_eregi)
--FILE--
<?php
var_dump(mb_eregi("a", "\xf5"));
?>
--EXPECTF--
bool(false)
