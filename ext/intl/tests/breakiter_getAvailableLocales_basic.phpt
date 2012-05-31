--TEST--
BreakIterator::getAvailableLocales(): basic test
--SKIPIF--
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(count(Breakiterator::getAvailableLocales()) > 150);
--EXPECT--
bool(true)
