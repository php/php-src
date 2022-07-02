--TEST--
IntlCalendar::get/getActualMaximum/getActualMinimum(): bad arguments (procedural)
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--FILE--
<?php

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

try {
    var_dump(intlcal_get($c));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(intlcal_get_actual_maximum($c));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(intlcal_get_actual_minimum($c));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(intlcal_get($c, -1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(intlcal_get_actual_maximum($c, -1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(intlcal_get_actual_minimum($c, -1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(intlcal_get($c, "s"));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(intlcal_get_actual_maximum($c, "s"));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(intlcal_get_actual_minimum($c, "s"));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(intlcal_get(1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(intlcal_get_actual_maximum(1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(intlcal_get_actual_minimum(1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
ArgumentCountError: 0, intlcal_get() expects exactly 2 arguments, 1 given
ArgumentCountError: 0, intlcal_get_actual_maximum() expects exactly 2 arguments, 1 given
ArgumentCountError: 0, intlcal_get_actual_minimum() expects exactly 2 arguments, 1 given
ValueError: 0, intlcal_get(): Argument #2 ($field) must be a valid field
ValueError: 0, intlcal_get_actual_maximum(): Argument #2 ($field) must be a valid field
ValueError: 0, intlcal_get_actual_minimum(): Argument #2 ($field) must be a valid field
TypeError: 0, intlcal_get(): Argument #2 ($field) must be of type int, string given
TypeError: 0, intlcal_get_actual_maximum(): Argument #2 ($field) must be of type int, string given
TypeError: 0, intlcal_get_actual_minimum(): Argument #2 ($field) must be of type int, string given
ArgumentCountError: 0, intlcal_get() expects exactly 2 arguments, 1 given
ArgumentCountError: 0, intlcal_get_actual_maximum() expects exactly 2 arguments, 1 given
ArgumentCountError: 0, intlcal_get_actual_minimum() expects exactly 2 arguments, 1 given
