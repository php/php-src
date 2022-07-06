--TEST--
invalid object raise exception() function
--SKIPIF--
<?php
if(!extension_loaded('enchant')) die('skip, enchant not loader');
if (!is_object(enchant_broker_init())) {die("skip, resource dont load\n");}
?>
--FILE--
<?php
$broker = enchant_broker_init();
if (is_object($broker)) {
	echo "OK\n";
	@enchant_broker_free($broker);
	try {
		@enchant_broker_free($broker);
	} catch (ValueError $e) {
		echo $e->getMessage()."\n";
	}
} else {
	exit("init failed\n");
}
echo "OK\n";
?>
--EXPECT--
OK
Invalid or uninitialized EnchantBroker object
OK
