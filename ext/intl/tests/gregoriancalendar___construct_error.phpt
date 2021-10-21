--TEST--
IntlGregorianCalendar::__construct(): bad arguments
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

try {
    var_dump(intlgregcal_create_instance(1,2,3,4,5,6,7));
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(new IntlGregorianCalendar(null, 1));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(new IntlGregorianCalendar(1, null));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(new IntlGregorianCalendar(1,2,NULL,4));
} catch (ValueError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(new IntlGregorianCalendar(1,2,3,4,5,array()));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}

$cal = new IntlGregorianCalendar();
try {
    $cal->__construct();
} catch (Error $e) {
    echo $e->getMessage(), "\n";
}
?>
--EXPECT--
intlgregcal_create_instance() expects at most 6 arguments, 7 given
IntlGregorianCalendar::__construct(): Argument #2 ($localeOrMonth) must be ?string if argument 1 is a timezone
IntlGregorianCalendar::__construct(): Argument #2 ($localeOrMonth) must be int if argument 1 is an int
IntlGregorianCalendar::__construct(): Argument #3 ($day) must be provided
IntlGregorianCalendar::__construct(): Argument #6 ($second) must be of type int, array given
IntlGregorianCalendar object is already constructed
