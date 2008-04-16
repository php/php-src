--TEST--
enchant_broker_init() function
--SKIPIF--
<?php 
if(!extension_loaded('enchant')) die('skip, enchant not loader');

 ?>
--FILE--
<?php
$broker = enchant_broker_init();
echo is_resource($broker) ? "OK" : "Failure";
echo "\n";
?>
--EXPECT--
OK
