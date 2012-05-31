--TEST--
BreakIterator::getAvailableLocales(): arg errors
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

var_dump(BreakIterator::getAvailableLocales(array()));

--EXPECTF--

Warning: BreakIterator::getAvailableLocales() expects exactly 0 parameters, 1 given in %s on line %d

Warning: BreakIterator::getAvailableLocales(): breakiter_get_available_locales: bad arguments in %s on line %d
bool(false)
