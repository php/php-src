--TEST--
Bug #62070: Collator::getSortKey() returns garbage
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '76.1') < 0) die('skip for ICU >= 76.1'); ?>
--FILE--
<?php
$s1 = 'Hello';

$coll = collator_create('en_US');
$res = collator_get_sort_key($coll, $s1);

echo urlencode($res);
?>
--EXPECT--
93AAG%01%09%01%DC%08
