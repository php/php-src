--TEST--
IntlCalendar::get/Least/Greatest/Minimum/Maximum(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

try {
    var_dump($c->getLeastMaximum(-1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
try {
    var_dump($c->getMaximum(-1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
try {
    var_dump($c->getGreatestMinimum(-1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
try {
    var_dump($c->getMinimum(-1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(intlcal_get_least_maximum($c, -1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(intlcal_get_maximum($c, -1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(intlcal_get_greatest_minimum($c, -1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(intlcal_get_minimum($c, -1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(intlcal_get_least_maximum(1, 1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(intlcal_get_maximum(1, 1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(intlcal_get_greatest_minimum(1, -1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(intlcal_get_minimum(1, -1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
ValueError: 0, IntlCalendar::getLeastMaximum(): Argument #1 ($field) must be a valid field
ValueError: 0, IntlCalendar::getMaximum(): Argument #1 ($field) must be a valid field
ValueError: 0, IntlCalendar::getGreatestMinimum(): Argument #1 ($field) must be a valid field
ValueError: 0, IntlCalendar::getMinimum(): Argument #1 ($field) must be a valid field
ValueError: 0, intlcal_get_least_maximum(): Argument #2 ($field) must be a valid field
ValueError: 0, intlcal_get_maximum(): Argument #2 ($field) must be a valid field
ValueError: 0, intlcal_get_greatest_minimum(): Argument #2 ($field) must be a valid field
ValueError: 0, intlcal_get_minimum(): Argument #2 ($field) must be a valid field
TypeError: 0, intlcal_get_least_maximum(): Argument #1 ($calendar) must be of type IntlCalendar, int given
TypeError: 0, intlcal_get_maximum(): Argument #1 ($calendar) must be of type IntlCalendar, int given
TypeError: 0, intlcal_get_greatest_minimum(): Argument #1 ($calendar) must be of type IntlCalendar, int given
TypeError: 0, intlcal_get_minimum(): Argument #1 ($calendar) must be of type IntlCalendar, int given
