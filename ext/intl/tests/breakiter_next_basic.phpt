--TEST--
IntlBreakIterator::next(): basic test
--EXTENSIONS--
intl
--FILE--
<?php

$bi = IntlBreakIterator::createWordInstance('pt');
$bi->setText('foo bar trans zoo bee');

var_dump($bi->first());
var_dump($bi->next());
var_dump($bi->next(2));
var_dump($bi->next(-1));
var_dump($bi->next(0));
var_dump($bi->next(NULL));
?>
--EXPECT--
int(0)
int(3)
int(7)
int(4)
int(4)
int(7)
