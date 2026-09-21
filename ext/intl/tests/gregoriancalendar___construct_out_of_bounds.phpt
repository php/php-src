--TEST--
IntlGregorianCalendar::__construct(): out-of-bounds date/time arguments report correct positions
--EXTENSIONS--
intl
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php
try {
    new IntlGregorianCalendar(99999999999, 1, 1);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    intlgregcal_create_instance(1, 99999999999, 1);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    new IntlGregorianCalendar(1, 1, 1, 99999999999, 1);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    new IntlGregorianCalendar(1, 1, 1, 1, 99999999999);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    intlgregcal_create_instance(1, 1, 1, 1, 1, 99999999999);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECTF--
Deprecated: Calling IntlGregorianCalendar::__construct() with more than 2 arguments is deprecated, use either IntlGregorianCalendar::createFromDate() or IntlGregorianCalendar::createFromDateTime() instead in %s on line %d
IntlGregorianCalendar::__construct(): Argument #1 ($timezoneOrYear) must be between -2147483648 and 2147483647

Deprecated: Function intlgregcal_create_instance() is deprecated since 8.4, use IntlGregorianCalendar::__construct(), IntlGregorianCalendar::createFromDate(), or IntlGregorianCalendar::createFromDateTime() instead in %s on line %d
intlgregcal_create_instance(): Argument #2 ($localeOrMonth) must be between -2147483648 and 2147483647

Deprecated: Calling IntlGregorianCalendar::__construct() with more than 2 arguments is deprecated, use either IntlGregorianCalendar::createFromDate() or IntlGregorianCalendar::createFromDateTime() instead in %s on line %d
IntlGregorianCalendar::__construct(): Argument #4 ($hour) must be between -2147483648 and 2147483647

Deprecated: Calling IntlGregorianCalendar::__construct() with more than 2 arguments is deprecated, use either IntlGregorianCalendar::createFromDate() or IntlGregorianCalendar::createFromDateTime() instead in %s on line %d
IntlGregorianCalendar::__construct(): Argument #5 ($minute) must be between -2147483648 and 2147483647

Deprecated: Function intlgregcal_create_instance() is deprecated since 8.4, use IntlGregorianCalendar::__construct(), IntlGregorianCalendar::createFromDate(), or IntlGregorianCalendar::createFromDateTime() instead in %s on line %d
intlgregcal_create_instance(): Argument #6 ($second) must be between -2147483648 and 2147483647
