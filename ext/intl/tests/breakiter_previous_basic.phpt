--TEST--
IntlBreakIterator::previous(): basic test
--EXTENSIONS--
intl
--FILE--
<?php

$bi = IntlBreakIterator::createWordInstance('pt');
$bi->setText('foo bar trans');

var_dump($bi->last());
var_dump($bi->previous());
?>
--EXPECT--
int(13)
int(8)
