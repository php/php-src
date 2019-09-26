--TEST--
Bug #52237 (Crash when passing the reference of the property of a non-object)
--FILE--
<?php
$data = [];
preg_match('//', '', $data[[]]);
var_dump(count($data));
?>
--EXPECTF--
Warning: Illegal offset type in %s on line %d
int(0)
