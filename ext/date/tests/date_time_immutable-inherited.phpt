--TEST--
Tests for DateTimeImmutable.
--INI--
date.timezone=Europe/London
--FILE--
<?php
$tz = new DateTimeZone("Asia/Tokyo");
$current = "2012-12-27 16:24:08";

echo "\ngetTimezone():\n";
$v = date_create_immutable($current);
$x = $v->getTimezone();
var_dump($x->getName());

echo "\ngetTimestamp():\n";
$v = date_create_immutable($current);
$x = $v->getTimestamp();
var_dump($x);
?>
--EXPECT--
getTimezone():
string(13) "Europe/London"

getTimestamp():
int(1356625448)
