--TEST--
IntlCalendar::setTimeZone(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

$gmt = IntlTimeZone::getGMT();

function eh($errno, $errstr) {
echo "error: $errno, $errstr\n";
}
set_error_handler('eh');

try {
    var_dump($c->setTimeZone($gmt, 2));
} catch (Error $ex) {
    echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try {
    var_dump($c->setTimeZone());
} catch (Error $ex) {
    echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}

try{
    var_dump(intlcal_set_time_zone($c, 1, 2));
} catch (Error $ex) {
    echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
try{
    var_dump(intlcal_set_time_zone(1, $gmt));
} catch (Error $ex) {
    echo "error: " . $ex->getCode() . ", " . $ex->getMessage() . "\n\n";
}
?>
--EXPECT--
error: 0, IntlCalendar::setTimeZone() expects exactly 1 argument, 2 given

error: 0, IntlCalendar::setTimeZone() expects exactly 1 argument, 0 given

error: 0, intlcal_set_time_zone() expects exactly 2 arguments, 3 given

error: 0, intlcal_set_time_zone(): Argument #1 ($calendar) must be of type IntlCalendar, int given
