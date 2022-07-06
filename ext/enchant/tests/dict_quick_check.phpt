--TEST--
enchant_dict_quick_check() basic test
--CREDITS--
marcosptf - <marcosptf@yahoo.com.br>
--SKIPIF--
<?php
if(!extension_loaded('enchant')) die('skip, enchant not loader');
if (getenv('SKIP_ASAN')) die('skip Known libenchant memory leak');
if (!is_object(enchant_broker_init())) {die("skip, resource dont load\n");}
if (!is_array(enchant_broker_list_dicts(enchant_broker_init()))) {die("skip, no dictionary installed on this machine! \n");}

$tag = 'en_US';
$r = enchant_broker_init();
if (!enchant_broker_dict_exists($r, $tag))
    die('skip, no dictionary for ' . $tag . ' tag');
?>
--FILE--
<?php

$tag = 'en_US';
$r = enchant_broker_init();

$d = enchant_broker_request_dict($r, $tag);
enchant_dict_quick_check($d, 'soong', $suggs);

echo "Elements: " . count($suggs) . "\n";
echo "Done\n";
?>
--EXPECTF--
Elements: %d
Done
