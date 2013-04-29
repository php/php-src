--TEST--
IntlTimeZone::createEnumeration(): variant with country
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
$tz = IntlTimeZone::createEnumeration('NL');
var_dump(get_class($tz));
$count = count(iterator_to_array($tz));
var_dump($count >= 1);

$tz->rewind();
var_dump(in_array('Europe/Amsterdam', iterator_to_array($tz)));

?>
==DONE==
--EXPECT--
string(12) "IntlIterator"
bool(true)
bool(true)
==DONE==