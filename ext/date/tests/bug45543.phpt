--TEST--
Test for bug #45543: DateTime::setTimezone can not set timezones without ID.
--INI--
date.timezone=UTC
--FILE--
<?php
$test_dates = array(
    '2008-01-01 12:00:00 PDT',
    '2008-01-01 12:00:00 +02:00',
);

foreach ($test_dates as $test_date)
{
    $d1 = new DateTime($test_date);
    $d2 = new DateTime('2008-01-01 12:00:00 UTC');
    echo $d1->format(DATE_ISO8601), PHP_EOL;
    echo $d2->format(DATE_ISO8601), PHP_EOL;
    $tz = $d1->getTimeZone();
    $d2->setTimeZone($tz);
    echo $d1->format(DATE_ISO8601), PHP_EOL;
    echo $d2->format(DATE_ISO8601), PHP_EOL;
    echo PHP_EOL;
}
?>
--EXPECT--
2008-01-01T12:00:00-0700
2008-01-01T12:00:00+0000
2008-01-01T12:00:00-0700
2008-01-01T05:00:00-0700

2008-01-01T12:00:00+0200
2008-01-01T12:00:00+0000
2008-01-01T12:00:00+0200
2008-01-01T14:00:00+0200
