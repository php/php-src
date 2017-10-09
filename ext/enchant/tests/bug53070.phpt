--TEST--
bug #53070, calling enchant_broker_get_path without enchant_broker_set_path leads to a segfault
--SKIPIF--
<?php
if(!extension_loaded('enchant')) die('skip, enchant not loader');
if (!is_resource(enchant_broker_init())) {die("skip, resource dont load\n");}
?>
--FILE--
<?php
$broker = enchant_broker_init();
var_dump(enchant_broker_get_dict_path($broker, ENCHANT_MYSPELL));
var_dump(enchant_broker_get_dict_path($broker, ENCHANT_ISPELL));
?>
--EXPECTF--

Warning: enchant_broker_get_dict_path(): dict_path not set in %s on line %d
bool(false)

Warning: enchant_broker_get_dict_path(): dict_path not set in %s on line %d
bool(false)
