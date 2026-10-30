--TEST--
IntlGregorianCalendar::setDateTime(): error cases
--EXTENSIONS--
intl
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php
try {
    var_dump(IntlGregorianCalendar::createFromDateTime(99999999999, 1, 1, 1, 1, 1));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(IntlGregorianCalendar::createFromDateTime(1, 99999999999, 1, 1, 1, 1));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(IntlGregorianCalendar::createFromDateTime(1, 1, 99999999999, 1, 1, 1));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(IntlGregorianCalendar::createFromDateTime(1, 1, 1, 99999999999, 1, 1));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(IntlGregorianCalendar::createFromDateTime(1, 1, 1, 1, 99999999999, 1));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(IntlGregorianCalendar::createFromDateTime(1, 1, 1, 1, 1, 99999999999));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
IntlGregorianCalendar::createFromDateTime(): Argument #1 ($year) must be between -2147483648 and 2147483647
IntlGregorianCalendar::createFromDateTime(): Argument #2 ($month) must be between -2147483648 and 2147483647
IntlGregorianCalendar::createFromDateTime(): Argument #3 ($dayOfMonth) must be between -2147483648 and 2147483647
IntlGregorianCalendar::createFromDateTime(): Argument #4 ($hour) must be between -2147483648 and 2147483647
IntlGregorianCalendar::createFromDateTime(): Argument #5 ($minute) must be between -2147483648 and 2147483647
IntlGregorianCalendar::createFromDateTime(): Argument #6 ($second) must be between -2147483648 and 2147483647
