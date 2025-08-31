--TEST--
IntlBreakIterator::getLocale(): basic test
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '64.0') < 0) die('skip for ICU >= 64.0'); ?>
--FILE--
<?php

$bi = IntlBreakIterator::createSentenceInstance('pt');

var_dump($bi->getLocale(0));
var_dump($bi->getLocale(1));
?>
--EXPECT--
string(0) ""
string(2) "pt"
