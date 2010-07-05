--TEST--
Bug #52237 (Crash when passing the reference of the property of a non-object)
--FILE--
<?php
$data = 'test';
preg_match('//', '', $data->info);
var_dump($data);
?>
--EXPECT--
string(4) "test"
