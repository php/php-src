--TEST--
IntlCalendar::setDate(): out-of-bounds arguments report correct positions
--EXTENSIONS--
intl
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php
$cal = IntlCalendar::createInstance();

try {
    $cal->setDate(99999999999, 1, 1);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $cal->setDate(1, 99999999999, 1);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $cal->setDate(1, 1, 99999999999);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
IntlCalendar::setDate(): Argument #1 ($year) must be between -2147483648 and 2147483647
IntlCalendar::setDate(): Argument #2 ($month) must be between -2147483648 and 2147483647
IntlCalendar::setDate(): Argument #3 ($dayOfMonth) must be between -2147483648 and 2147483647
