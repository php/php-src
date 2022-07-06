--TEST--
enchant_broker_free() function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
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
    enchant_broker_free($broker);

} else {
    exit("init failed\n");
}
echo "OK\n";
?>
--EXPECTF--
OK

Deprecated: Function enchant_broker_free() is deprecated in %s
OK
