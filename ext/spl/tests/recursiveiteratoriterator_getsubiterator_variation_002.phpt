--TEST--
SPL: RecursiveIteratorIterator::getSubIterator() returns NULL if there's no current element
--CREDITS--
Matt Raines matt@raines.me.uk
#testfest London 2009-05-09
--FILE--
<?php
$sample_array = array(1);

$iterator = new RecursiveIteratorIterator(new RecursiveArrayIterator($sample_array));

$iterator->next();
var_dump(is_null($iterator->getSubIterator()));
$iterator->next();
var_dump(is_null($iterator->getSubIterator()));
?>
--EXPECT--
bool(false)
bool(false)
