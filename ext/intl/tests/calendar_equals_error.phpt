--TEST--
IntlCalendar::equals(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

function eh($errno, $errstr) {
echo "error: $errno, $errstr\n";
}
set_error_handler('eh');

try {
    var_dump($c->equals());
} catch (Error $ex) {
    echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
    var_dump($c->equals(new stdclass));
} catch (Error $ex) {
    echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
    var_dump($c->equals(1, 2));
} catch (Error $ex) {
    echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}


try {
    var_dump(intlcal_equals($c, array()));
} catch (Error $ex) {
    echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
    var_dump(intlcal_equals(1, $c));
} catch (Error $ex) {
    echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
?>
--EXPECT--
error: 0, IntlCalendar::equals() expects exactly 1 argument, 0 given

error: 0, IntlCalendar::equals(): Argument #1 ($other) must be of type IntlCalendar, stdClass given

error: 0, IntlCalendar::equals() expects exactly 1 argument, 2 given

error: 0, intlcal_equals(): Argument #2 ($other) must be of type IntlCalendar, array given

error: 0, intlcal_equals(): Argument #1 ($calendar) must be of type IntlCalendar, int given
