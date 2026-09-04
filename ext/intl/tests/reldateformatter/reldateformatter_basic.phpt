--TEST--
IntlRelativeDateTimeFormatter basic formatting
--EXTENSIONS--
intl
--FILE--
<?php

$formatter = new IntlRelativeDateTimeFormatter('en_US');

echo $formatter->format(-1, IntlRelativeDateTimeFormatter::UNIT_DAY), "\n";
echo $formatter->format(0, IntlRelativeDateTimeFormatter::UNIT_DAY), "\n";
echo $formatter->format(1, IntlRelativeDateTimeFormatter::UNIT_DAY), "\n";
echo $formatter->format(-1, IntlRelativeDateTimeFormatter::UNIT_WEEK), "\n";
echo $formatter->format(2, IntlRelativeDateTimeFormatter::UNIT_WEEK), "\n";
echo $formatter->format(-1, IntlRelativeDateTimeFormatter::UNIT_SUNDAY), "\n";

echo $formatter->formatNumeric(-1, IntlRelativeDateTimeFormatter::UNIT_DAY), "\n";
echo $formatter->formatNumeric(1, IntlRelativeDateTimeFormatter::UNIT_DAY), "\n";
echo $formatter->formatNumeric(1.5, IntlRelativeDateTimeFormatter::UNIT_HOUR), "\n";

$relativeDate = $formatter->format(-1, IntlRelativeDateTimeFormatter::UNIT_DAY);
echo $formatter->combineDateAndTime($relativeDate, '3:45 PM'), "\n";

var_dump($formatter->getErrorCode());
var_dump($formatter->getErrorMessage());

?>
--EXPECTF--
yesterday
today
tomorrow
last week
in 2 weeks
last Sunday
1 day ago
in 1 day
in 1.5 hours
yesterday%s3:45 PM
int(0)
string(12) "U_ZERO_ERROR"
