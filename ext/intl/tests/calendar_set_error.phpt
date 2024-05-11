--TEST--
IntlCalendar::set(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

try {
    $c->set(1, 2, 3, 4, 5, 6, 7);
} catch (ArgumentCountError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    $c->set(1, 2, 3, 4);
} catch (ArgumentCountError $exception) {
    echo $exception->getMessage() . "\n";
}

try {
    var_dump($c->set(-1, 2));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(intlcal_set($c, -1, 2));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(intlcal_set(1, 2, 3));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECTF--
Deprecated: Calling IntlCalendar::set() with more than 2 arguments is deprecated, use either IntlCalendar::setDate() or IntlCalendar::setDateTime() instead in %s on line %d
IntlCalendar::set() expects at most 6 arguments, 7 given

Deprecated: Calling IntlCalendar::set() with more than 2 arguments is deprecated, use either IntlCalendar::setDate() or IntlCalendar::setDateTime() instead in %s on line %d
IntlCalendar::set() has no variant with exactly 4 parameters
IntlCalendar::set(): Argument #1 ($year) must be a valid field

Deprecated: Function intlcal_set() is deprecated in %s on line %d
intlcal_set(): Argument #2 ($year) must be a valid field

Deprecated: Function intlcal_set() is deprecated in %s on line %d
intlcal_set(): Argument #1 ($calendar) must be of type IntlCalendar, int given
