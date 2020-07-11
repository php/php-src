--TEST--
IntlGregorianCalendar::__construct(): bad arguments
--SKIPIF--
<?php
if (!extension_loaded('intl'))
	die('skip intl extension not enabled');
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
    var_dump(new IntlGregorianCalendar(1,2,3,4,NULL,array()));
} catch (TypeError $e) {
    echo $e->getMessage(), "\n";
}
--EXPECT--
Too many arguments
Too many arguments
No variant with 4 arguments (excluding trailing NULLs)
No variant with 4 arguments (excluding trailing NULLs)
IntlGregorianCalendar::__construct(): Argument #6 must be of type int, array given
