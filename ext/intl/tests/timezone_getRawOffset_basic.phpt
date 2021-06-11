--TEST--
IntlTimeZone::getRawOffset(): basic test
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);
$ams = IntlTimeZone::createTimeZone('Europe/Amsterdam');
var_dump($ams->getRawOffset());

$lsb = IntlTimeZone::createTimeZone('Europe/Lisbon');
var_dump(intltz_get_raw_offset($lsb));

?>
--EXPECT--
int(3600000)
int(0)