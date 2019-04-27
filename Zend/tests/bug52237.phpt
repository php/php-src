--TEST--
Bug #52237 (Crash when passing the reference of the property of a non-object)
--FILE--
<?php
$data = 'test';
preg_match('//', '', $data->info);
var_dump($data);
?>
--EXPECTF--
Warning: Attempt to modify property 'info' of non-object in %sbug52237.php on line 3
string(4) "test"
