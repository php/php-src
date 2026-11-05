--TEST--
enchant_broker_set_dict_path() / enchant_broker_get_dict_path() honour their return types without libenchant 1 support
--EXTENSIONS--
enchant
--SKIPIF--
<?php
if (!defined("LIBENCHANT_VERSION") || version_compare(LIBENCHANT_VERSION, "2", "<")) die('skip libenchant v2 only');
?>
--FILE--
<?php
$broker = enchant_broker_init();
var_dump(@enchant_broker_set_dict_path($broker, ENCHANT_MYSPELL, '/tmp'));
var_dump(@enchant_broker_get_dict_path($broker, ENCHANT_MYSPELL));
?>
--EXPECT--
bool(false)
bool(false)
