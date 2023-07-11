--TEST--
Bug GH-11600: Intl patterns are not parseable DateTime Strings
--INI--
date.timezone=UTC
--FILE--
<?php
$formatter = new IntlDateFormatter('en_US', -1, 3);
$pattern = $formatter->getPattern();

$timeString = $formatter->format(strtotime('2023-07-11 16:02'));

$timestamp = strtotime($timeString);

var_dump($pattern, $timeString, $timestamp);
?>
--EXPECTF--
string(8) "h:mm a"
string(9) "4:02 PM"
int(1689091320)
