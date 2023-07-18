--TEST--
Bug GH-11600: Intl patterns are not parseable DateTime Strings
--EXTENSIONS--
intl
--INI--
date.timezone=UTC
--FILE--
<?php
$formatter = new IntlDateFormatter('en_US', -1, 3);
$pattern = $formatter->getPattern();

$timeString = $formatter->format(strtotime('2023-07-11 16:02'));

$timestamp = strtotime("2023-07-11 {$timeString}");

var_dump($pattern, $timeString, $timestamp);
?>
--EXPECTF--
string(%d) "h:mm%sa"
string(%d) "4:02%sPM"
int(1689091320)
