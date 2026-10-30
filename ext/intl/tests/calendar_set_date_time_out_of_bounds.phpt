--TEST--
IntlCalendar::setDateTime(): out-of-bounds arguments report correct positions
--EXTENSIONS--
intl
--SKIPIF--
<?php if (PHP_INT_SIZE != 8) die("skip: 64-bit only"); ?>
--FILE--
<?php
$cal = IntlCalendar::createInstance();

try {
    $cal->setDateTime(99999999999, 1, 1, 1, 1, 1);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $cal->setDateTime(1, 99999999999, 1, 1, 1, 1);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $cal->setDateTime(1, 1, 99999999999, 1, 1, 1);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $cal->setDateTime(1, 1, 1, 99999999999, 1, 1);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $cal->setDateTime(1, 1, 1, 1, 99999999999, 1);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}

try {
    $cal->setDateTime(1, 1, 1, 1, 1, 99999999999);
} catch (Throwable $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
IntlCalendar::setDateTime(): Argument #1 ($year) must be between -2147483648 and 2147483647
IntlCalendar::setDateTime(): Argument #2 ($month) must be between -2147483648 and 2147483647
IntlCalendar::setDateTime(): Argument #3 ($dayOfMonth) must be between -2147483648 and 2147483647
IntlCalendar::setDateTime(): Argument #4 ($hour) must be between -2147483648 and 2147483647
IntlCalendar::setDateTime(): Argument #5 ($minute) must be between -2147483648 and 2147483647
IntlCalendar::setDateTime(): Argument #6 ($second) must be between -2147483648 and 2147483647
