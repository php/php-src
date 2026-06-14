--TEST--
IntlTimeZone::createEnumeration(): variant with country
--EXTENSIONS--
intl
--FILE--
<?php
$tz = IntlTimeZone::createEnumeration('NL');
var_dump(get_class($tz));
$count = count(iterator_to_array($tz));
var_dump($count >= 1);

$tz->rewind();
var_dump(in_array('Europe/Amsterdam', iterator_to_array($tz)));

?>
--EXPECT--
string(12) "IntlIterator"
bool(true)
bool(true)
