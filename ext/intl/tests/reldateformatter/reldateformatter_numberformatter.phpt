--TEST--
IntlRelativeDateTimeFormatter observes changes to a custom NumberFormatter
--EXTENSIONS--
intl
--FILE--
<?php

$numberFormatter = new NumberFormatter('en_US', NumberFormatter::DECIMAL);
$numberFormatter->setAttribute(NumberFormatter::MIN_FRACTION_DIGITS, 1);
$formatter = new IntlRelativeDateTimeFormatter(
    'en_US',
    IntlRelativeDateTimeFormatterStyle::Long,
    IntlRelativeDateTimeFormatterCapitalization::None,
    $numberFormatter,
);

echo $formatter->formatNumeric(2, IntlRelativeDateTimeFormatterUnit::Day), "\n";
echo $formatter->format(2, IntlRelativeDateTimeFormatterUnit::Week), "\n";

$numberFormatter->setAttribute(NumberFormatter::MIN_FRACTION_DIGITS, 0);

echo $formatter->formatNumeric(2, IntlRelativeDateTimeFormatterUnit::Day), "\n";
echo $formatter->format(2, IntlRelativeDateTimeFormatterUnit::Week), "\n";

unset($numberFormatter);
echo $formatter->formatNumeric(2, IntlRelativeDateTimeFormatterUnit::Day), "\n";

?>
--EXPECT--
in 2.0 days
in 2.0 weeks
in 2 days
in 2 weeks
in 2 days
