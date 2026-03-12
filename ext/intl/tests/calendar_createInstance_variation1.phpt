--TEST--
IntlCalendar::createInstance() argument variations
--EXTENSIONS--
intl
--INI--
date.timezone=Europe/Amsterdam
intl.default_locale=nl
--FILE--
<?php

$cal = intlcal_create_instance('Europe/Amsterdam');
print_R($cal->getTimeZone());
print_R($cal->getLocale(Locale::ACTUAL_LOCALE));
echo "\n";

$cal = intlcal_create_instance('Europe/Lisbon', null);
print_R($cal->getTimeZone());
print_R($cal->getLocale(Locale::ACTUAL_LOCALE));
echo "\n";

$cal = intlcal_create_instance(IntlTimeZone::createTimeZone('Europe/Lisbon'));
print_R($cal->getTimeZone());
print_R($cal->getLocale(Locale::ACTUAL_LOCALE));
echo "\n";

$cal = intlcal_create_instance(null, "pt");
print_R($cal->getTimeZone());
print_R($cal->getLocale(Locale::ACTUAL_LOCALE));
echo "\n";

$cal = intlcal_create_instance("Europe/Lisbon", "pt");
print_R($cal->getTimeZone());
print_R($cal->getLocale(Locale::ACTUAL_LOCALE));
echo "\n";

?>
--EXPECTF--
IntlTimeZone Object
(
    [valid] => 1
    [id] => Europe/Amsterdam
    [rawOffset] => 3600000
    [currentOffset] => %d
)
nl
IntlTimeZone Object
(
    [valid] => 1
    [id] => Europe/Lisbon
    [rawOffset] => 0
    [currentOffset] => %d
)
nl
IntlTimeZone Object
(
    [valid] => 1
    [id] => Europe/Lisbon
    [rawOffset] => 0
    [currentOffset] => %d
)
nl
IntlTimeZone Object
(
    [valid] => 1
    [id] => Europe/Amsterdam
    [rawOffset] => 3600000
    [currentOffset] => %d
)
pt
IntlTimeZone Object
(
    [valid] => 1
    [id] => Europe/Lisbon
    [rawOffset] => 0
    [currentOffset] => %d
)
pt
