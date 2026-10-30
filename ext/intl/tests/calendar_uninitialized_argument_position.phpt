--TEST--
IntlCalendar methods report the correct argument position for uninitialized calendar arguments
--EXTENSIONS--
intl
--FILE--
<?php

$calendar = IntlGregorianCalendar::createFromDate(2024, 0, 1);
$uninitialized = (new ReflectionClass(IntlGregorianCalendar::class))->newInstanceWithoutConstructor();

foreach (['equals', 'before', 'after', 'isEquivalentTo'] as $method) {
    try {
        $calendar->$method($uninitialized);
    } catch (Error $e) {
        echo $method, ': ', $e->getMessage(), PHP_EOL;
    }
}

foreach (['intlcal_equals', 'intlcal_before', 'intlcal_after', 'intlcal_is_equivalent_to'] as $function) {
    try {
        $function($calendar, $uninitialized);
    } catch (Error $e) {
        echo $function, ': ', $e->getMessage(), PHP_EOL;
    }
}

?>
--EXPECT--
equals: IntlCalendar::equals(): Argument #1 ($other) is uninitialized
before: IntlCalendar::before(): Argument #1 ($other) is uninitialized
after: IntlCalendar::after(): Argument #1 ($other) is uninitialized
isEquivalentTo: IntlCalendar::isEquivalentTo(): Argument #1 ($other) is uninitialized
intlcal_equals: intlcal_equals(): Argument #2 ($other) is uninitialized
intlcal_before: intlcal_before(): Argument #2 ($other) is uninitialized
intlcal_after: intlcal_after(): Argument #2 ($other) is uninitialized
intlcal_is_equivalent_to: intlcal_is_equivalent_to(): Argument #2 ($other) is uninitialized
