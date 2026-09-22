--TEST--
IntlRelativeDateTimeFormatter basic formatting
--EXTENSIONS--
intl
--FILE--
<?php

$formatter = new IntlRelativeDateTimeFormatter('en_US');

echo $formatter->format(-1, IntlRelativeDateTimeFormatterUnit::Day), "\n";
echo $formatter->format(0, IntlRelativeDateTimeFormatterUnit::Day), "\n";
echo $formatter->format(1, IntlRelativeDateTimeFormatterUnit::Day), "\n";
echo $formatter->format(-1, IntlRelativeDateTimeFormatterUnit::Week), "\n";
echo $formatter->format(2, IntlRelativeDateTimeFormatterUnit::Week), "\n";
echo $formatter->format(-1, IntlRelativeDateTimeFormatterUnit::Sunday), "\n";

echo $formatter->formatNumeric(-1, IntlRelativeDateTimeFormatterUnit::Day), "\n";
echo $formatter->formatNumeric(1, IntlRelativeDateTimeFormatterUnit::Day), "\n";
echo $formatter->formatNumeric(1.5, IntlRelativeDateTimeFormatterUnit::Hour), "\n";

$relativeDate = $formatter->format(-1, IntlRelativeDateTimeFormatterUnit::Day);
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
