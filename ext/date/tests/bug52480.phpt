--TEST--
Bug #52480 (Incorrect difference using DateInterval)
--XFAIL--
See https://bugs.php.net/bug.php?id=52480
--FILE--
<?php

$expectedDiff = (array) new DateInterval('P30D');

// If the DateInterval object was created by DateTime::diff(), then this is the total
// number of days between the start and end dates. Otherwise, days will be FALSE.
// https://secure.php.net/manual/en/class.dateinterval.php
$expectedDiff['days'] = 30;

foreach (DateTimeZone::listIdentifiers() as $timezone) {
    $start = new DateTime('2017-03-01', new DateTimeZone($timezone));
    $end = new DateTime('2017-03-31', new DateTimeZone($timezone));

    if ($expectedDiff != (array) $start->diff($end)) {
        echo "\nWrong result for $timezone!\n";
    }
}
?>
===DONE===
--EXPECT--
===DONE===
