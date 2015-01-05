--TEST--
enchant_broker_free() function
--SKIPIF--
<?php 
if(!extension_loaded('enchant')) die('skip, enchant not loader');

 ?>
--FILE--
<?php
$broker = enchant_broker_init();
if (is_resource($broker)) {
	echo "OK\n";
	enchant_broker_free($broker);
}  else {
	exit("init failed\n");
}
echo "OK\n";
?>
--EXPECT--
OK
OK
