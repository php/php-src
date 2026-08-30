--TEST--
IntlRelativeDateTimeFormatter locales, styles, and capitalization context
--EXTENSIONS--
intl
--FILE--
<?php

$french = new IntlRelativeDateTimeFormatter('fr_FR');
echo $french->formatNumeric(-1, IntlRelativeDateTimeFormatter::UNIT_DAY), "\n";
echo $french->formatNumeric(1.5, IntlRelativeDateTimeFormatter::UNIT_HOUR), "\n";

$chinese = new IntlRelativeDateTimeFormatter('zh_CN');
echo $chinese->formatNumeric(3, IntlRelativeDateTimeFormatter::UNIT_MONTH), "\n";

$beginning = new IntlRelativeDateTimeFormatter(
    'en_US',
    IntlRelativeDateTimeFormatter::STYLE_LONG,
    IntlRelativeDateTimeFormatter::CAPITALIZATION_FOR_BEGINNING_OF_SENTENCE,
);
echo $beginning->format(-1, IntlRelativeDateTimeFormatter::UNIT_DAY), "\n";

$short = new IntlRelativeDateTimeFormatter(
    'en_US',
    IntlRelativeDateTimeFormatter::STYLE_SHORT,
);
$narrow = new IntlRelativeDateTimeFormatter(
    'en_US',
    IntlRelativeDateTimeFormatter::STYLE_NARROW,
);
echo $short->formatNumeric(3, IntlRelativeDateTimeFormatter::UNIT_DAY), "\n";
echo $narrow->formatNumeric(3, IntlRelativeDateTimeFormatter::UNIT_DAY), "\n";

Locale::setDefault('en_US');
$defaultLocale = new IntlRelativeDateTimeFormatter();
echo $defaultLocale->format(1, IntlRelativeDateTimeFormatter::UNIT_DAY), "\n";

?>
--EXPECT--
il y a 1 jour
dans 1,5 heure
3个月后
Yesterday
in 3 days
in 3d
tomorrow
