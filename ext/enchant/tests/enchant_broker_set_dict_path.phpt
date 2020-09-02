--TEST--
proto bool enchant_broker_set_dict_path(resource broker, int dict_type, string value) function
proto string enchant_broker_get_dict_path(resource broker, int dict_type) function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php
if(!extension_loaded('enchant')) die('skip, enchant not loader');
if (!is_object(enchant_broker_init())) {die("skip, resource dont load\n");}
if (!is_array(enchant_broker_list_dicts(enchant_broker_init()))) {die("skip, no dictionary installed on this machine! \n");}
if (defined("LIBENCHANT_VERSION") && version_compare(LIBENCHANT_VERSION, "2", ">")) die('skip libenchant v1 only');
?>
--FILE--
<?php
$broker = enchant_broker_init();
$backEndDictType1 = "MYSPELL";
$backEndDictType2 = "ISPELL";
$dictTypeValue1 = 1;
$dictTypeValue2 = 2;

if (is_object($broker)) {
    echo("OK\n");

    if (enchant_broker_set_dict_path($broker, $dictTypeValue1, $backEndDictType1)) {
        echo("OK\n");

        if (enchant_broker_set_dict_path($broker, $dictTypeValue2, $backEndDictType2)) {
            echo("OK\n");

            if (
                  (enchant_broker_get_dict_path($broker,$dictTypeValue1) == $backEndDictType1) &&
                  (enchant_broker_get_dict_path($broker,$dictTypeValue2) == $backEndDictType2)
              ) {
                   echo("OK\n");

            } else {
                   echo("broker get dict path has failed \n");
            }

        } else {
           echo("broker set dict path {$backEndDictType2} has failed \n");
        }
    } else {
        echo("broker set dict path {$backEndDictType1} has failed \n");
    }
} else {
    echo("broker is not a resource; failed; \n");
}
?>
--EXPECTF--
OK

Deprecated: Function enchant_broker_set_dict_path() is deprecated in %s
OK

Deprecated: Function enchant_broker_set_dict_path() is deprecated in %s
OK

Deprecated: Function enchant_broker_get_dict_path() is deprecated in %s

Deprecated: Function enchant_broker_get_dict_path() is deprecated in %s
OK
