--TEST--
Bug #74468 Wrong reflection on Collator::sortWithSortKeys
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '51.2') >=  0) die('skip for ICU < 51.2'); ?>
--FILE--
<?php
$rm = new ReflectionMethod(Collator::class, 'sortWithSortKeys');
var_dump($rm->getNumberOfParameters());
var_dump($rm->getNumberOfRequiredParameters());

$rf = new ReflectionFunction('collator_sort_with_sort_keys');
var_dump($rf->getNumberOfParameters());
var_dump($rf->getNumberOfRequiredParameters());
?>
--EXPECT--
int(1)
int(1)
int(2)
int(2)
