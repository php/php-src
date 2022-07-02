--TEST--
IntlCalendar::setSkipped/RepeatedWallTimeOption(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

try {
    var_dump($c->setSkippedWallTimeOption(3));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump($c->setRepeatedWallTimeOption(2));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(intlcal_set_repeated_wall_time_option(1, 1));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
IntlCalendar::setSkippedWallTimeOption(): Argument #1 ($option) must be one of IntlCalendar::WALLTIME_FIRST, IntlCalendar::WALLTIME_LAST, or IntlCalendar::WALLTIME_NEXT_VALID
IntlCalendar::setRepeatedWallTimeOption(): Argument #1 ($option) must be either IntlCalendar::WALLTIME_FIRST or IntlCalendar::WALLTIME_LAST
intlcal_set_repeated_wall_time_option(): Argument #1 ($calendar) must be of type IntlCalendar, int given
