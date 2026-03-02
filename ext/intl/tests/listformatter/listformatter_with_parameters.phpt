--TEST--
IntlListFormatter: Test AND, OR and Width parameters
--EXTENSIONS--
intl
--SKIPIF--
<?php if (version_compare(INTL_ICU_VERSION, '67.0') <= 0) die('skip for ICU < 67.0'); ?>
--FILE--
<?php

echo 'EN_US' .PHP_EOL;

$formatter = new IntlListFormatter('EN_US', IntlListFormatter::TYPE_AND, IntlListFormatter::WIDTH_WIDE);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('EN_US', IntlListFormatter::TYPE_AND, IntlListFormatter::WIDTH_SHORT);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('EN_US', IntlListFormatter::TYPE_AND, IntlListFormatter::WIDTH_NARROW);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('EN_US', IntlListFormatter::TYPE_OR, IntlListFormatter::WIDTH_WIDE);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('EN_US', IntlListFormatter::TYPE_OR, IntlListFormatter::WIDTH_SHORT);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('EN_US', IntlListFormatter::TYPE_OR, IntlListFormatter::WIDTH_NARROW);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('EN_US', IntlListFormatter::TYPE_UNITS, IntlListFormatter::WIDTH_WIDE);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('EN_US', IntlListFormatter::TYPE_UNITS, IntlListFormatter::WIDTH_SHORT);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('EN_US', IntlListFormatter::TYPE_UNITS, IntlListFormatter::WIDTH_NARROW);
echo $formatter->format([1,2,3]) . PHP_EOL;

echo 'GB' . PHP_EOL;

$formatter = new IntlListFormatter('en_GB', IntlListFormatter::TYPE_AND, IntlListFormatter::WIDTH_SHORT);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('en_GB', IntlListFormatter::TYPE_AND, IntlListFormatter::WIDTH_SHORT);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('en_GB', IntlListFormatter::TYPE_AND, IntlListFormatter::WIDTH_NARROW);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('en_GB', IntlListFormatter::TYPE_OR, IntlListFormatter::WIDTH_WIDE);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('en_GB', IntlListFormatter::TYPE_OR, IntlListFormatter::WIDTH_SHORT);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('en_GB', IntlListFormatter::TYPE_OR, IntlListFormatter::WIDTH_NARROW);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('en_GB', IntlListFormatter::TYPE_UNITS, IntlListFormatter::WIDTH_WIDE);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('en_GB', IntlListFormatter::TYPE_UNITS, IntlListFormatter::WIDTH_SHORT);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('en_GB', IntlListFormatter::TYPE_UNITS, IntlListFormatter::WIDTH_NARROW);
echo $formatter->format([1,2,3]) . PHP_EOL;

echo 'FR' . PHP_EOL;

$formatter = new IntlListFormatter('FR', IntlListFormatter::TYPE_AND, IntlListFormatter::WIDTH_SHORT);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('FR', IntlListFormatter::TYPE_AND, IntlListFormatter::WIDTH_SHORT);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('FR', IntlListFormatter::TYPE_AND, IntlListFormatter::WIDTH_NARROW);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('FR', IntlListFormatter::TYPE_OR, IntlListFormatter::WIDTH_WIDE);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('FR', IntlListFormatter::TYPE_OR, IntlListFormatter::WIDTH_SHORT);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('FR', IntlListFormatter::TYPE_OR, IntlListFormatter::WIDTH_NARROW);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('FR', IntlListFormatter::TYPE_UNITS, IntlListFormatter::WIDTH_WIDE);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('FR', IntlListFormatter::TYPE_UNITS, IntlListFormatter::WIDTH_SHORT);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('FR', IntlListFormatter::TYPE_UNITS, IntlListFormatter::WIDTH_NARROW);
echo $formatter->format([1,2,3]);
?>
--EXPECT--
EN_US
1, 2, and 3
1, 2, & 3
1, 2, 3
1, 2, or 3
1, 2, or 3
1, 2, or 3
1, 2, 3
1, 2, 3
1 2 3
GB
1, 2 and 3
1, 2 and 3
1, 2, 3
1, 2 or 3
1, 2 or 3
1, 2 or 3
1, 2, 3
1, 2, 3
1 2 3
FR
1, 2 et 3
1, 2 et 3
1, 2, 3
1, 2 ou 3
1, 2 ou 3
1, 2 ou 3
1, 2 et 3
1, 2 et 3
1 2 3
