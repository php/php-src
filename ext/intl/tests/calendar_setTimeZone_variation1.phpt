--TEST--
IntlCalendar::setTimeZone() variation with NULL arg
--EXTENSIONS--
intl
--FILE--
<?php

$intlcal = IntlCalendar::createInstance('Europe/Amsterdam');
print_r($intlcal->getTimeZone()->getID());
echo "\n";
var_dump($intlcal->get(IntlCalendar::FIELD_ZONE_OFFSET));

/* passing NULL has no effect */
$intlcal->setTimeZone(null);
print_r($intlcal->getTimeZone()->getID());
echo "\n";
var_dump($intlcal->get(IntlCalendar::FIELD_ZONE_OFFSET));

?>
--EXPECT--
Europe/Amsterdam
int(3600000)
Europe/Amsterdam
int(3600000)
