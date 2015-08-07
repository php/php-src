--TEST--
enchant_broker_list_dicts() function
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php 
if(!extension_loaded('enchant')) die('skip, enchant not loader');
?>
--FILE--
<?php
$broker = enchant_broker_init();
if (is_resource($broker)) {
    var_dump(enchant_broker_list_dicts($broker));
} else {
    exit("init failed\n");
}

?>
--EXPECTF--
array(%d) {
  [%d]=>
  array(%d) {
    ["lang_tag"]=>
    string(%d) "en_US"
    ["provider_name"]=>
    string(%d) "myspell"
    ["provider_desc"]=>
    string(%d) "Myspell Provider"
    ["provider_file"]=>
    string(%d) "/usr/lib64/enchant/libenchant_myspell.so"
  }
}
