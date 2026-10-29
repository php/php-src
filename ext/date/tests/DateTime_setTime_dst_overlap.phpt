--TEST--
DateTime::setTime() preserves the offset in an overlapping local time
--FILE--
<?php

foreach ([
    ['Europe/Dublin', '2026-10-25 00:10:30 UTC', 1, 10],
    ['Europe/London', '2026-10-25 00:10:30 UTC', 1, 10],
    ['Europe/Amsterdam', '2026-10-25 00:10:30 UTC', 2, 10],
    ['America/New_York', '2026-11-01 05:10:30 UTC', 1, 10],
    ['Australia/Lord_Howe', '2026-04-04 14:45:30 UTC', 1, 45],
] as [$timezone, $instant, $hour, $minute]) {
    $date = new DateTimeImmutable($instant);
    $date = $date->setTimezone(new DateTimeZone($timezone));
    $result = $date->setTime($hour, $minute, 59);
    printf("%s %s %+d\n", $timezone, $result->format('H:i:s T'), $result->getTimestamp() - $date->getTimestamp());
}

$date = new DateTime('2026-10-25 01:10:30 UTC');
$date->setTimezone(new DateTimeZone('Europe/Dublin'));
$date->setTime(1, 10, 59);
printf("%s %+d\n", $date->format('H:i:s T'), $date->getTimestamp() - strtotime('2026-10-25 01:10:30 UTC'));

$date = new DateTimeImmutable('2026-10-25 01:10:30 UTC');
$date = $date->setTimezone(new DateTimeZone('Europe/Dublin'));
$result = $date->setTime(1, 10, 59);
printf("%s %+d\n", $result->format('H:i:s T'), $result->getTimestamp() - $date->getTimestamp());

$date = new DateTimeImmutable('2026-10-25 01:10:30 UTC');
$date = $date->setTimezone(new DateTimeZone('Europe/London'));
$result = $date->modify('+29 seconds');
printf("%s %+d\n", $result->format('H:i:s T'), $result->getTimestamp() - $date->getTimestamp());

$date = new DateTimeImmutable('2026-10-25 00:10:30 UTC');
$date = $date->setTimezone(new DateTimeZone('Europe/Dublin'));
$result = $date->modify('+29 seconds');
printf("%s %+d\n", $result->format('H:i:s T'), $result->getTimestamp() - $date->getTimestamp());

?>
--EXPECT--
Europe/Dublin 01:10:59 IST +29
Europe/London 01:10:59 BST +29
Europe/Amsterdam 02:10:59 CEST +29
America/New_York 01:10:59 EDT +29
Australia/Lord_Howe 01:45:59 +11 +29
01:10:59 GMT +29
01:10:59 GMT +29
01:10:59 GMT +29
01:10:59 IST +29
