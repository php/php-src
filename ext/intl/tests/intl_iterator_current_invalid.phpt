--TEST--
IntlIterator::current() returns null when the iterator is not valid
--EXTENSIONS--
intl
--FILE--
<?php
$iterator = IntlBreakIterator::createWordInstance('en_US')->getPartsIterator();
var_dump($iterator->current());

$breakIterator = IntlBreakIterator::createWordInstance('en_US');
$breakIterator->setText('foo');
$iterator = $breakIterator->getPartsIterator();
$iterator->rewind();
$iterator->next();
$iterator->next();
var_dump($iterator->valid());
var_dump($iterator->current());
?>
--EXPECT--
NULL
bool(false)
NULL
