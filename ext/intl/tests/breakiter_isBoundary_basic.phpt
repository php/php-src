--TEST--
IntlBreakIterator::isBoundary(): basic test
--EXTENSIONS--
intl
--FILE--
<?php

$bi = IntlBreakIterator::createWordInstance('pt');
$bi->setText('foo bar trans zoo bee');

var_dump($bi->isBoundary(0));
var_dump($bi->isBoundary(7));
var_dump($bi->isBoundary(-1));
var_dump($bi->isBoundary(1));
var_dump($bi->isBoundary(50));
?>
--EXPECT--
bool(true)
bool(true)
bool(false)
bool(false)
bool(false)
