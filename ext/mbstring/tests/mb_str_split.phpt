--TEST--
mb_str_split()
--SKIPIF--
<?php extension_loaded('mbstring') or die('skip mbstring not available'); ?>
--INI--
output_handler=
mbstring.func_overload=0
--FILE--
<?php
ini_set('include_path','.');
include_once('common.inc');

// Japanese
$utf8_jp = "どうも";
$euc_jp = mb_convert_encoding($utf8_jp, "EUC-JP", "UTF-8");

// Russian
$utf8_ru_1 = "Миру";
$utf8_ru_2 = "Мир";
$cp866_ru = mb_convert_encoding($utf8_ru_2, "CP866", "UTF-8");


printf("1: %s\n", serialize(mb_str_split($utf8_ru_1, 1 , "UTF-8")));
printf("2: %s\n", mb_convert_encoding(serialize(mb_str_split($cp866_ru, 1 , "CP866")), "UTF-8", "CP866"));
printf("3: %s\n", mb_convert_encoding(serialize(mb_str_split($euc_jp, 1, "EUC-JP")), "UTF-8", "EUC-JP"));
printf("4: %s\n", mb_convert_encoding(serialize(mb_str_split($euc_jp, 3, "EUC-JP")), "UTF-8", "EUC-JP"));


?>
--EXPECT--
1: a:4:{i:0;s:2:"М";i:1;s:2:"и";i:2;s:2:"р";i:3;s:2:"у";}
2: a:3:{i:0;s:1:"М";i:1;s:1:"и";i:2;s:1:"р";}
3: a:3:{i:0;s:2:"ど";i:1;s:2:"う";i:2;s:2:"も";}
4: a:1:{i:0;s:6:"どうも";}
