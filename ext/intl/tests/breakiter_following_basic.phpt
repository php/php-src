--TEST--
IntlBreakIterator::following(): basic test
--EXTENSIONS--
intl
--FILE--
<?php

$bi = IntlBreakIterator::createWordInstance('pt');
$bi->setText('foo bar trans zoo bee');

var_dump($bi->following(5));
var_dump($bi->following(50));
var_dump($bi->following(-1));
?>
--EXPECT--
int(7)
int(-1)
int(0)
