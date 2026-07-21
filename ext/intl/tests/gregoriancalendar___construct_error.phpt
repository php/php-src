--TEST--
IntlGregorianCalendar::__construct(): bad arguments
--EXTENSIONS--
intl
--FILE--
<?php

try {
    var_dump(intlgregcal_create_instance(1,2,3,4,5,6,7));
} catch (ArgumentCountError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(intlgregcal_create_instance(1,2,3,4,5,6,7,8));
} catch (ArgumentCountError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(intlgregcal_create_instance(1,2,3,4));
} catch (ArgumentCountError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(new IntlGregorianCalendar(1,2,NULL,4));
} catch (ArgumentCountError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump(new IntlGregorianCalendar(1,2,3,4,5,array()));
} catch (TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

$cal = new IntlGregorianCalendar();
try {
    $cal->__construct();
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
?>
--EXPECTF--
Deprecated: Function intlgregcal_create_instance() is deprecated since 8.4, use IntlGregorianCalendar::__construct(), IntlGregorianCalendar::createFromDate(), or IntlGregorianCalendar::createFromDateTime() instead in %s on line %d
ArgumentCountError: Too many arguments

Deprecated: Function intlgregcal_create_instance() is deprecated since 8.4, use IntlGregorianCalendar::__construct(), IntlGregorianCalendar::createFromDate(), or IntlGregorianCalendar::createFromDateTime() instead in %s on line %d
ArgumentCountError: Too many arguments

Deprecated: Function intlgregcal_create_instance() is deprecated since 8.4, use IntlGregorianCalendar::__construct(), IntlGregorianCalendar::createFromDate(), or IntlGregorianCalendar::createFromDateTime() instead in %s on line %d
ArgumentCountError: No variant with 4 arguments (excluding trailing NULLs)

Deprecated: Calling IntlGregorianCalendar::__construct() with more than 2 arguments is deprecated, use either IntlGregorianCalendar::createFromDate() or IntlGregorianCalendar::createFromDateTime() instead in %s on line %d
ArgumentCountError: No variant with 4 arguments (excluding trailing NULLs)

Deprecated: Calling IntlGregorianCalendar::__construct() with more than 2 arguments is deprecated, use either IntlGregorianCalendar::createFromDate() or IntlGregorianCalendar::createFromDateTime() instead in %s on line %d
TypeError: IntlGregorianCalendar::__construct(): Argument #6 ($second) must be of type int, array given
Error: IntlGregorianCalendar object is already constructed
