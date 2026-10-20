--TEST--
Test mysqli_object_has_property() internal object handler
--EXTENSIONS--
mysqli
--FILE--
<?php
$driver = new mysqli_driver();
var_dump(isset($driver->client_info));
var_dump(empty($driver->client_info));
?>
--EXPECT--
bool(true)
bool(false)
