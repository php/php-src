--TEST--
IntlRelativeDateTimeFormatter locales, styles, and capitalization context
--EXTENSIONS--
intl
--FILE--
<?php

$french = new IntlRelativeDateTimeFormatter('fr_FR');
echo $french->formatNumeric(-1, IntlRelativeDateTimeFormatterUnit::Day), "\n";
echo $french->formatNumeric(1.5, IntlRelativeDateTimeFormatterUnit::Hour), "\n";

$chinese = new IntlRelativeDateTimeFormatter('zh_CN');
echo $chinese->formatNumeric(3, IntlRelativeDateTimeFormatterUnit::Month), "\n";

$beginning = new IntlRelativeDateTimeFormatter(
    'en_US',
    IntlRelativeDateTimeFormatterStyle::Long,
    IntlRelativeDateTimeFormatterCapitalization::BeginningOfSentence,
);
echo $beginning->format(-1, IntlRelativeDateTimeFormatterUnit::Day), "\n";

$short = new IntlRelativeDateTimeFormatter(
    'en_US',
    IntlRelativeDateTimeFormatterStyle::Short,
);
$narrow = new IntlRelativeDateTimeFormatter(
    'en_US',
    IntlRelativeDateTimeFormatterStyle::Narrow,
);
echo $short->formatNumeric(3, IntlRelativeDateTimeFormatterUnit::Day), "\n";
echo $narrow->formatNumeric(3, IntlRelativeDateTimeFormatterUnit::Day), "\n";

Locale::setDefault('en_US');
$defaultLocale = new IntlRelativeDateTimeFormatter();
echo $defaultLocale->format(1, IntlRelativeDateTimeFormatterUnit::Day), "\n";

?>
--EXPECT--
il y a 1 jour
dans 1,5 heure
3个月后
Yesterday
in 3 days
in 3d
tomorrow
