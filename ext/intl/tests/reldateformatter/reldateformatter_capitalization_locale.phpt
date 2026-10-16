--TEST--
IntlRelativeDateTimeFormatter sentence capitalization uses the locale and preserves existing case
--EXTENSIONS--
intl
--FILE--
<?php

$numberFormatter = new NumberFormatter('tr', NumberFormatter::DECIMAL);
$numberFormatter->setTextAttribute(NumberFormatter::POSITIVE_PREFIX, 'istanbul ABC ');
$formatter = new IntlRelativeDateTimeFormatter(
    'tr',
    IntlRelativeDateTimeFormatterStyle::Long,
    IntlRelativeDateTimeFormatterCapitalization::BeginningOfSentence,
    $numberFormatter,
);

echo $formatter->format(-1, IntlRelativeDateTimeFormatterUnit::Day), "\n";
echo $formatter->format(3, IntlRelativeDateTimeFormatterUnit::Day), "\n";
echo $formatter->formatNumeric(2, IntlRelativeDateTimeFormatterUnit::Day), "\n";

$numberFormatter->setTextAttribute(NumberFormatter::POSITIVE_PREFIX, 'izmir XYZ ');
echo $formatter->formatNumeric(2, IntlRelativeDateTimeFormatterUnit::Day), "\n";

// Combining strings must not apply sentence capitalization.
echo $formatter->combineDateAndTime('istanbul', '12:00'), "\n";

$numberFormatter->setTextAttribute(NumberFormatter::POSITIVE_PREFIX, '');
echo $formatter->formatNumeric(2, IntlRelativeDateTimeFormatterUnit::Day), "\n";

?>
--EXPECT--
Dün
İstanbul ABC 3 gün sonra
İstanbul ABC 2 gün sonra
İzmir XYZ 2 gün sonra
istanbul 12:00
2 gün sonra
