--TEST--
IntlBreakIterator::getPartsIterator(): bad args
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
ini_set("intl.default_locale", "pt_PT");

$it = IntlBreakIterator::createWordInstance(NULL);
var_dump($it->getPartsIterator(-1));

?>
--EXPECTF--
Warning: IntlBreakIterator::getPartsIterator(): breakiter_get_parts_iterator: bad key type in %s on line %d
bool(false)
