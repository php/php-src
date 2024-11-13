--TEST--
IntlGregorianCalendar::setDate(): error cases
--EXTENSIONS--
intl
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php
try {
    var_dump(IntlGregorianCalendar::createFromDate(99999999999, 1, 1));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(IntlGregorianCalendar::createFromDate(1, 99999999999, 1));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

try {
    var_dump(IntlGregorianCalendar::createFromDate(1, 1, 99999999999));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}

?>
--EXPECT--
IntlGregorianCalendar::createFromDate(): Argument #1 ($year) must be between -2147483648 and 2147483647
IntlGregorianCalendar::createFromDate(): Argument #2 ($month) must be between -2147483648 and 2147483647
IntlGregorianCalendar::createFromDate(): Argument #3 ($dayOfMonth) must be between -2147483648 and 2147483647
