--TEST--
IntlTimeZone::getDSTSavings(): basic test
--EXTENSIONS--
intl
--FILE--
<?php

$lsb = IntlTimeZone::createTimeZone('Europe/Lisbon');
var_dump($lsb->getDSTSavings());

var_dump(intltz_get_dst_savings($lsb));

?>
--EXPECT--
int(3600000)
int(3600000)
