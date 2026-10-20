--TEST--
null bytes
--EXTENSIONS--
enchant
--SKIPIF--
<?php
if (!enchant_broker_list_dicts(enchant_broker_init())) {die("skip no dictionary installed on this machine! \n");}
?>
--FILE--
<?php

$broker = enchant_broker_init();
$dicts = enchant_broker_list_dicts($broker);
$requestDict = enchant_broker_request_dict($broker, $dicts[0]['lang_tag']);

$two_params_broker = [
    "enchant_broker_request_dict",
    "enchant_broker_dict_exists",
];

$two_params_dict = [
    "enchant_dict_quick_check",
    "enchant_dict_check",
    "enchant_dict_suggest",
    "enchant_dict_add",
    "enchant_dict_add_to_session",
    "enchant_dict_remove_from_session",
    "enchant_dict_is_added",
];

foreach ($two_params_broker as $func) {
    try {
        $func($broker, "foo\0bar");
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}

foreach ($two_params_dict as $func) {
    try {
        $func($requestDict, "foo\0bar");
    } catch (ValueError $e) {
        echo $e->getMessage(), "\n";
    }
}

try {
    var_dump(enchant_broker_set_ordering($broker, "foo\0bar", "foo\0bar"));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(enchant_dict_store_replacement($requestDict, "foo\0bar", "foo\0bar"));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
enchant_broker_request_dict(): Argument #2 ($tag) must not contain any null bytes
enchant_broker_dict_exists(): Argument #2 ($tag) must not contain any null bytes
enchant_dict_quick_check(): Argument #2 ($word) must not contain any null bytes
enchant_dict_check(): Argument #2 ($word) must not contain any null bytes
enchant_dict_suggest(): Argument #2 ($word) must not contain any null bytes
enchant_dict_add(): Argument #2 ($word) must not contain any null bytes
enchant_dict_add_to_session(): Argument #2 ($word) must not contain any null bytes
enchant_dict_remove_from_session(): Argument #2 ($word) must not contain any null bytes
enchant_dict_is_added(): Argument #2 ($word) must not contain any null bytes
enchant_broker_set_ordering(): Argument #2 ($tag) must not contain any null bytes
enchant_dict_store_replacement(): Argument #2 ($misspelled) must not contain any null bytes
