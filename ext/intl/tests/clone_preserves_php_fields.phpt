--TEST--
Cloning IntlDateFormatter and MessageFormatter preserves PHP-side fields
--EXTENSIONS--
intl
--FILE--
<?php
$d = new IntlDateFormatter('de_DE', IntlDateFormatter::LONG, IntlDateFormatter::MEDIUM, 'UTC', IntlDateFormatter::GREGORIAN);
$c = clone $d;
var_dump($c->getDateType());
var_dump($c->getTimeType());
var_dump($c->getCalendar());
var_dump($c->format(strtotime('2024-01-02 03:04:05 UTC')));
$c->setCalendar(IntlDateFormatter::GREGORIAN);
var_dump($c->getPattern());

$m = new MessageFormatter('en_US', '{0, number}');
$mc = clone $m;
var_dump($mc->getPattern());
var_dump($mc->format([1.5]));

date_default_timezone_set('UTC');
$t = new MessageFormatter('en_US', '{0,time,short}');
$dt = new DateTimeImmutable('2024-01-02 03:04:05', new DateTimeZone('UTC'));
$t->format([$dt]);
$tc = clone $t;
date_default_timezone_set('America/New_York');
var_dump($t->format([$dt]) === $tc->format([$dt]));
?>
--EXPECT--
int(1)
int(2)
int(1)
string(26) "2. Januar 2024 um 03:04:05"
string(23) "d. MMMM y 'um' HH:mm:ss"
string(11) "{0, number}"
string(3) "1.5"
bool(true)
