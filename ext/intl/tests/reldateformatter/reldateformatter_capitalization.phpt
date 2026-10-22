--TEST--
IntlRelativeDateTimeFormatter capitalization enums and NumberFormatter refresh
--EXTENSIONS--
intl
--FILE--
<?php

$numberFormatter = new NumberFormatter('en_US', NumberFormatter::DECIMAL);
foreach (IntlRelativeDateTimeFormatterCapitalization::cases() as $context) {
    $numberFormatter->setAttribute(NumberFormatter::MIN_FRACTION_DIGITS, 0);
    $formatter = new IntlRelativeDateTimeFormatter(
        'en_US',
        IntlRelativeDateTimeFormatterStyle::Narrow,
        $context,
        $numberFormatter,
    );
    echo $context->name, ': ', $formatter->format(-1, IntlRelativeDateTimeFormatterUnit::Day), "\n";
    echo $formatter->formatNumeric(2, IntlRelativeDateTimeFormatterUnit::Day), "\n";

    $numberFormatter->setAttribute(NumberFormatter::MIN_FRACTION_DIGITS, 1);
    echo $formatter->format(-1, IntlRelativeDateTimeFormatterUnit::Day), "\n";
    echo $formatter->formatNumeric(2, IntlRelativeDateTimeFormatterUnit::Day), "\n";
}

?>
--EXPECTF--
None: yesterday
in 2%r(?:d| days)%r
yesterday
in 2.0%r(?:d| days)%r
MiddleOfSentence: yesterday
in 2%r(?:d| days)%r
yesterday
in 2.0%r(?:d| days)%r
BeginningOfSentence: Yesterday
In 2%r(?:d| days)%r
Yesterday
In 2.0%r(?:d| days)%r
UiListAndMenu: yesterday
in 2%r(?:d| days)%r
yesterday
in 2.0%r(?:d| days)%r
Standalone: yesterday
in 2%r(?:d| days)%r
yesterday
in 2.0%r(?:d| days)%r
