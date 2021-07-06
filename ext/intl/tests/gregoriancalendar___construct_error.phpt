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
    var_dump(intlgregcal_create_instance(1,2,3,4,5,6,7,8));
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(intlgregcal_create_instance(1,2,3,4));
} catch (ArgumentCountError $e) {
    echo $e->getMessage(), "\n";
}
try {
    var_dump(new IntlGregorianCalendar(1,2,NULL,4));
} catch (ArgumentCountError $e) {
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
Too many arguments
Too many arguments
No variant with 4 arguments (excluding trailing NULLs)
No variant with 4 arguments (excluding trailing NULLs)
IntlGregorianCalendar::__construct(): Argument #6 ($second) must be of type int, array given
IntlGregorianCalendar object is already constructed
