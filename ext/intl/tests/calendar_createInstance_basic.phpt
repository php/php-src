--TEST--
IntlCalendar::createInstance() basic test
--EXTENSIONS--
intl
--INI--
intl.default_locale=nl
date.timezone=Europe/Amsterdam
--FILE--
<?php

$cal = IntlCalendar::createInstance();
print_R($cal->getTimeZone());
print_R($cal->getLocale(Locale::ACTUAL_LOCALE));
echo "\n";
print_R($cal->getType());
echo "\n";

$timeMillis = $cal->getTime();
$time = time();

var_dump(abs($timeMillis - $time * 1000) < 2000);

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
gregorian
bool(true)
