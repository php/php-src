--TEST--
date_format(): time zone characters, escaped and not, on every kind of zone
--INI--
date.timezone=UTC
--FILE--
<?php
$dates = [
    new DateTimeImmutable('2026-07-01 12:00:00', new DateTimeZone('UTC')),
    new DateTimeImmutable('2026-07-01 12:00:00', new DateTimeZone('Europe/Madrid')),
    new DateTimeImmutable('2026-11-18T12:34:56-09:30'),
    new DateTimeImmutable('2026-11-18 12:34:56 EST'),
];
foreach ($dates as $d) {
    var_dump($d->format('\I\O\P\p\T\e\Z\c\r Y-m-d\TH:i:s'));
    var_dump($d->format('\\\\T \TT'));
    var_dump($d->format('e p T P O Z I'));
    var_dump($d->format('Y-m-d H:i:s T (P) e'));
    var_dump($d->format('c | r'));
}
var_dump(gmdate('e p T P O Z I c', 1763469296));
?>
--EXPECT--
string(29) "IOPpTeZcr 2026-07-01T12:00:00"
string(9) "\UTC TUTC"
string(26) "UTC Z UTC +00:00 +0000 0 0"
string(36) "2026-07-01 12:00:00 UTC (+00:00) UTC"
string(59) "2026-07-01T12:00:00+00:00 | Wed, 01 Jul 2026 12:00:00 +0000"
string(29) "IOPpTeZcr 2026-07-01T12:00:00"
string(11) "\CEST TCEST"
string(45) "Europe/Madrid +02:00 CEST +02:00 +0200 7200 1"
string(47) "2026-07-01 12:00:00 CEST (+02:00) Europe/Madrid"
string(59) "2026-07-01T12:00:00+02:00 | Wed, 01 Jul 2026 12:00:00 +0200"
string(29) "IOPpTeZcr 2026-11-18T12:34:56"
string(19) "\GMT-0930 TGMT-0930"
string(44) "-09:30 -09:30 GMT-0930 -09:30 -0930 -34200 0"
string(44) "2026-11-18 12:34:56 GMT-0930 (-09:30) -09:30"
string(59) "2026-11-18T12:34:56-09:30 | Wed, 18 Nov 2026 12:34:56 -0930"
string(29) "IOPpTeZcr 2026-11-18T12:34:56"
string(9) "\EST TEST"
string(36) "EST -05:00 EST -05:00 -0500 -18000 0"
string(36) "2026-11-18 12:34:56 EST (-05:00) EST"
string(59) "2026-11-18T12:34:56-05:00 | Wed, 18 Nov 2026 12:34:56 -0500"
string(52) "UTC Z GMT +00:00 +0000 0 0 2025-11-18T12:34:56+00:00"
