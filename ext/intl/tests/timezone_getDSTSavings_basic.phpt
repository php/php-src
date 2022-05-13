--TEST--
IntlTimeZone::getDSTSavings(): basic test
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$lsb = IntlTimeZone::createTimeZone('Europe/Lisbon');
var_dump($lsb->getDSTSavings());

var_dump(intltz_get_dst_savings($lsb));

?>
--EXPECT--
int(3600000)
int(3600000)