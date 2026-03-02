--TEST--
IntlCalendar::setSkipped/RepeatedWallTimeOption(): bad arguments
--EXTENSIONS--
intl
--FILE--
<?php

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

?>
--EXPECT--
IntlCalendar::setSkippedWallTimeOption(): Argument #1 ($option) must be one of IntlCalendar::WALLTIME_FIRST, IntlCalendar::WALLTIME_LAST, or IntlCalendar::WALLTIME_NEXT_VALID
IntlCalendar::setRepeatedWallTimeOption(): Argument #1 ($option) must be either IntlCalendar::WALLTIME_FIRST or IntlCalendar::WALLTIME_LAST
