--TEST--
constructor raise exception()
--SKIPIF--
<?php
if(!extension_loaded('enchant')) die('skip, enchant not loader');
if (!is_object(enchant_broker_init())) {die("skip, resource dont load\n");}
?>
--INI--
open_basedir=.
--FILE--
<?php
try {
	new EnchantDict(new EnchantBroker, '');
} catch (ValueError $e) {
	echo $e->getMessage()."\n";
}
try {
	new EnchantDict(new EnchantBroker, '', '../test.pwl');
} catch (Exception $e) {
	echo $e->getMessage()."\n";
}
?>
OK
--EXPECTF--
Tag cannot be empty
Open_basedir restriction in effect
OK
