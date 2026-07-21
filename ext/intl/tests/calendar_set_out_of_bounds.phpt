--TEST--
IntlCalendar::set(): out-of-bounds date/time arguments report correct positions
--EXTENSIONS--
intl
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php
$cal = IntlCalendar::createInstance();

try {
    $cal->set(99999999999, 1, 1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    intlcal_set($cal, 1, 99999999999, 1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $cal->set(1, 1, 1, 99999999999, 1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    $cal->set(1, 1, 1, 1, 99999999999, 1);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

try {
    intlcal_set($cal, 1, 1, 1, 1, 1, 99999999999);
} catch (Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
Deprecated: Calling IntlCalendar::set() with more than 2 arguments is deprecated, use either IntlCalendar::setDate() or IntlCalendar::setDateTime() instead in %s on line %d
ValueError: IntlCalendar::set(): Argument #1 ($year) must be between -2147483648 and 2147483647

Deprecated: Function intlcal_set() is deprecated since 8.4, use IntlCalendar::set(), IntlCalendar::setDate(), or IntlCalendar::setDateTime() instead in %s on line %d
ValueError: intlcal_set(): Argument #3 ($month) must be between -2147483648 and 2147483647

Deprecated: Calling IntlCalendar::set() with more than 2 arguments is deprecated, use either IntlCalendar::setDate() or IntlCalendar::setDateTime() instead in %s on line %d
ValueError: IntlCalendar::set(): Argument #4 ($hour) must be between -2147483648 and 2147483647

Deprecated: Calling IntlCalendar::set() with more than 2 arguments is deprecated, use either IntlCalendar::setDate() or IntlCalendar::setDateTime() instead in %s on line %d
ValueError: IntlCalendar::set(): Argument #5 ($minute) must be between -2147483648 and 2147483647

Deprecated: Function intlcal_set() is deprecated since 8.4, use IntlCalendar::set(), IntlCalendar::setDate(), or IntlCalendar::setDateTime() instead in %s on line %d
ValueError: intlcal_set(): Argument #7 ($second) must be between -2147483648 and 2147483647
