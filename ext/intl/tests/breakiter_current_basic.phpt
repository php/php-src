--TEST--
IntlBreakIterator::current(): basic test
--EXTENSIONS--
intl
--FILE--
<?php

$bi = IntlBreakIterator::createWordInstance('pt');
var_dump($bi->current());
$bi->setText('foo bar trans zoo bee');

var_dump($bi->first());
var_dump($bi->current());
var_dump($bi->next());
var_dump($bi->current());
?>
--EXPECT--
int(0)
int(0)
int(0)
int(3)
int(3)
