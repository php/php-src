--TEST--
IntlListFormatter: Basic functionality
--EXTENSIONS--
intl
--FILE--
<?php

echo 'EN_US' .PHP_EOL;

$formatter = new IntlListFormatter('EN_US', IntlListFormatter::TYPE_AND, IntlListFormatter::WIDTH_WIDE);
echo $formatter->format([1,2,3]) . PHP_EOL;
$formatter = new IntlListFormatter('EN_US');
echo $formatter->format([1,2,3]) . PHP_EOL;

echo 'FR' . PHP_EOL;

$formatter = new IntlListFormatter('FR', IntlListFormatter::TYPE_AND, IntlListFormatter::WIDTH_WIDE);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('FR');
echo $formatter->format([1,2,3]) . PHP_EOL;

--EXPECT--
EN_US
1, 2, and 3
1, 2, and 3
FR
1, 2 et 3
1, 2 et 3