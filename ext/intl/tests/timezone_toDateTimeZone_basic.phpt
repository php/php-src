--TEST--
IntlTimeZone::toDateTimeZone(): basic test
--EXTENSIONS--
intl
--INI--
date.timezone=Europe/Lisbon
intl.default_locale=nl
--FILE--
<?php

function do_test(IntlTimeZone $tz, $proc = false) {
    var_dump($tz->getID(), $tz->getRawOffset());
    if (!$proc)
        $dtz = $tz->toDateTimeZone();
    else
        $dtz = intltz_to_date_time_zone($tz);
    var_dump($dtz->getName(), $dtz->getOffset(new DateTime('2012-01-01 00:00:00')));
}

do_test(IntlTimeZone::createTimeZone('CET'));
do_test(IntlTimeZone::createTimeZone('Europe/Amsterdam'));
do_test(IntlTimeZone::createTimeZone('GMT+0405'), true);
?>
--EXPECTF--
string(3) "CET"
int(3600000)
string(%d) "%rEurope\/Berlin|CET%r"
int(3600)
string(16) "Europe/Amsterdam"
int(3600000)
string(16) "Europe/Amsterdam"
int(3600)
string(%s) "GMT+04%s5"
int(14700000)
string(6) "+04:05"
int(14700)
