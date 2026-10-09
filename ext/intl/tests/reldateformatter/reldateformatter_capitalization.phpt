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
--EXPECT--
None: yesterday
in 2d
yesterday
in 2.0d
MiddleOfSentence: yesterday
in 2d
yesterday
in 2.0d
BeginningOfSentence: Yesterday
In 2d
Yesterday
In 2.0d
UiListAndMenu: yesterday
in 2d
yesterday
in 2.0d
Standalone: yesterday
in 2d
yesterday
in 2.0d
