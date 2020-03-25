--TEST--
Bug #62070: Collator::getSortKey() returns garbage
--SKIPIF--
<?php if (!extension_loaded('intl')) die('skip intl extension not enabled'); ?>
<?php if (version_compare(INTL_ICU_VERSION, '62.1') < 0) die('skip for ICU >= 62.1'); ?>
--FILE--
<?php
$s1 = 'Hello';

$coll = collator_create('en_US');
$res = collator_get_sort_key($coll, $s1);

echo urlencode($res);
--EXPECT--
82%40%40F%01%09%01%DC%08
