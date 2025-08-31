--TEST--
Bug #71516 IntlDateFormatter losts locale if pattern is set via constructor
--EXTENSIONS--
intl
--FILE--
<?php

$loc = "ru_RU";
$goodFormatter = new IntlDateFormatter($loc, IntlDateFormatter::FULL, IntlDateFormatter::FULL, new DateTimeZone("UTC"));
$badFormatter  = new IntlDateFormatter($loc, IntlDateFormatter::FULL, IntlDateFormatter::FULL, new DateTimeZone("UTC"), null, "d MMM");
$badFormatter2 = new IntlDateFormatter($loc, IntlDateFormatter::FULL, IntlDateFormatter::FULL, new DateTimeZone("UTC"));
$badFormatter2->setPattern("d MMM");

echo "Formatter without pattern: " . $goodFormatter->getLocale() . PHP_EOL;
echo "Formatter with pattern: " . $badFormatter->getLocale() . PHP_EOL;
echo "Formatter with pattern set later: " . $badFormatter2->getLocale() . PHP_EOL;

?>
--EXPECT--
Formatter without pattern: ru
Formatter with pattern: ru
Formatter with pattern set later: ru
