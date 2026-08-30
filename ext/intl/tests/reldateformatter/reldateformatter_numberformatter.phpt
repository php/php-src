--TEST--
IntlRelativeDateTimeFormatter clones a custom NumberFormatter
--EXTENSIONS--
intl
--FILE--
<?php

$numberFormatter = new NumberFormatter('en_US', NumberFormatter::DECIMAL);
$numberFormatter->setAttribute(NumberFormatter::MIN_FRACTION_DIGITS, 1);
$formatter = new IntlRelativeDateTimeFormatter(
    'en_US',
    IntlRelativeDateTimeFormatter::STYLE_LONG,
    IntlRelativeDateTimeFormatter::CAPITALIZATION_NONE,
    $numberFormatter,
);

/* Mutating and destroying the source formatter must not affect the clone. */
$numberFormatter->setAttribute(NumberFormatter::MIN_FRACTION_DIGITS, 0);
unset($numberFormatter);

echo $formatter->formatNumeric(2, IntlRelativeDateTimeFormatter::UNIT_DAY), "\n";
echo $formatter->format(2, IntlRelativeDateTimeFormatter::UNIT_WEEK), "\n";

?>
--EXPECT--
in 2.0 days
in 2.0 weeks
