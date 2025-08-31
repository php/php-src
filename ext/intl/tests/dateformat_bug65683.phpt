--TEST--
Bug #65683 IntlDateFormatter accepts DateTimeImmutable
--EXTENSIONS--
intl
--FILE--
<?php

$formatter = new IntlDateFormatter('en-US', IntlDateFormatter::FULL, IntlDateFormatter::NONE, new DateTimeZone("UTC"));
var_dump($formatter->format(new DateTimeImmutable('2017-03-27 00:00:00 UTC'))) . "\n";

?>
--EXPECTF--
string(%s) "Monday, March %d, 2017"
