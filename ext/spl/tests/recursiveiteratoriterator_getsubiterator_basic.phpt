--TEST--
SPL: RecursiveIteratorIterator::getSubIterator() returns iterator passed in constructor
--CREDITS--
Matt Raines matt@raines.me.uk
#testfest London 2009-05-09
--FILE--
<?php
$sample_array = array(1, 2, array(3, 4));

$sub_iterator = new RecursiveArrayIterator($sample_array);
$not_sub_iterator = new RecursiveArrayIterator($sample_array);
$iterator = new RecursiveIteratorIterator($sub_iterator);

var_dump($iterator->getSubIterator() === $sub_iterator);
var_dump($iterator->getSubIterator() === $not_sub_iterator);
?>
--EXPECT--
bool(true)
bool(false)
