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

echo $formatter->format([1.2,2.3,3.4]) . PHP_EOL;

$item = 'test';
$item2 = 'test2';
$item3 = &$item;
$items = [$item, $item2, $item3];
$items2 = &$items;

echo $formatter->format($items) . PHP_EOL;
echo $formatter->format($items2) . PHP_EOL;

echo $formatter->format([null, true, false]) . PHP_EOL;

$classItem = new class {
    public function __toString() {
        return 'foo';
    }
};

echo $formatter->format([1, $classItem]) . PHP_EOL;


echo 'FR' . PHP_EOL;

$formatter = new IntlListFormatter('FR', IntlListFormatter::TYPE_AND, IntlListFormatter::WIDTH_WIDE);
echo $formatter->format([1,2,3]) . PHP_EOL;

$formatter = new IntlListFormatter('FR');
echo $formatter->format([1,2,3]) . PHP_EOL;

// Make it clear that numbers are not converted automatically to the locale. Use NumberFormatter for each value.
echo $formatter->format([1.2,2.3,3.4]) . PHP_EOL;
?>
--EXPECT--
EN_US
1, 2, and 3
1, 2, and 3
1.2, 2.3, and 3.4
test, test2, and test
test, test2, and test
, 1, and 
1 and foo
FR
1, 2 et 3
1, 2 et 3
1.2, 2.3 et 3.4
