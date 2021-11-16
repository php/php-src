--TEST--
Bug #53070 (enchant_broker_get_path crashes if no path is set)
--EXTENSIONS--
enchant
--SKIPIF--
<?php
if (!is_object(enchant_broker_init())) {die("skip, resource dont load\n");}
if (defined("LIBENCHANT_VERSION") && version_compare(LIBENCHANT_VERSION, "2", ">")) die('skip libenchant v1 only');
?>
--FILE--
<?php
$broker = enchant_broker_init();
var_dump(enchant_broker_get_dict_path($broker, ENCHANT_MYSPELL));
var_dump(enchant_broker_get_dict_path($broker, ENCHANT_ISPELL));
?>
--EXPECTF--
Deprecated: Constant ENCHANT_MYSPELL is deprecated in %s

Deprecated: Function enchant_broker_get_dict_path() is deprecated in %s

Warning: enchant_broker_get_dict_path(): dict_path not set in %s on line %d
bool(false)

Deprecated: Constant ENCHANT_ISPELL is deprecated in %s

Deprecated: Function enchant_broker_get_dict_path() is deprecated in %s

Warning: enchant_broker_get_dict_path(): dict_path not set in %s on line %d
bool(false)
