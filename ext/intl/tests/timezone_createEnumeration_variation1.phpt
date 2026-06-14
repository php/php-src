--TEST--
IntlTimeZone::createEnumeration(): variant with offset
--EXTENSIONS--
intl
--FILE--
<?php
$tz = IntlTimeZone::createEnumeration(3600000);
var_dump(get_class($tz));
$count = count(iterator_to_array($tz));
var_dump($count > 20);

$tz->rewind();
var_dump(in_array('Europe/Amsterdam', iterator_to_array($tz)));

?>
--EXPECT--
string(12) "IntlIterator"
bool(true)
bool(true)
