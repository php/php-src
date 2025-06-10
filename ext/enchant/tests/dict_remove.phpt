--TEST--
enchant_dict_remove() function
--EXTENSIONS--
enchant
--SKIPIF--
<?php
if (!enchant_broker_list_dicts(enchant_broker_init())) die("skip no dictionary installed on this machine");
?>
--FILE--
<?php
$broker = enchant_broker_init();
$dicts = enchant_broker_list_dicts($broker);
$newWord = 'myImaginaryWord';

$requestDict = enchant_broker_request_dict($broker, $dicts[0]['lang_tag']);

var_dump(enchant_dict_check($requestDict, $newWord));
enchant_dict_add($requestDict, $newWord);
var_dump(enchant_dict_check($requestDict, $newWord));
var_dump(enchant_dict_is_added($requestDict, $newWord));
enchant_dict_remove($requestDict, $newWord);
var_dump(enchant_dict_check($requestDict, $newWord));
var_dump(enchant_dict_is_added($requestDict, $newWord));
?>
--EXPECT--
bool(false)
bool(true)
bool(true)
bool(false)
bool(false)
