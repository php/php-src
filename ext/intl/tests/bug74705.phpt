--TEST--
Bug #74705 Wrong reflection on Collator::getSortKey
--SKIPIF--
<?php if( !extension_loaded( 'intl' ) ) print 'skip'; ?>
--FILE--
<?php
$rm = new ReflectionMethod(Collator::class, 'getSortKey');
var_dump($rm->getNumberOfParameters());
var_dump($rm->getNumberOfRequiredParameters());

$rf = new ReflectionFunction('collator_get_sort_key');
var_dump($rf->getNumberOfParameters());
var_dump($rf->getNumberOfRequiredParameters());
?>
--EXPECT--
int(1)
int(1)
int(2)
int(2)
