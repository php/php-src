--TEST--
IntlCalendar::setMinimalDaysInFirstWeek(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--SKIPIF--
<?php
if (!extension_loaded('intl'))
    die('skip intl extension not enabled');
--FILE--
<?php

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

try {
    var_dump($c->setMinimalDaysInFirstWeek(0));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(intlcal_set_minimal_days_in_first_week($c, 0));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(intlcal_set_minimal_days_in_first_week(1, 2));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
IntlCalendar::setMinimalDaysInFirstWeek(): Argument #1 ($days) must be between 1 and 7
intlcal_set_minimal_days_in_first_week(): Argument #2 ($days) must be between 1 and 7
intlcal_set_minimal_days_in_first_week(): Argument #1 ($calendar) must be of type IntlCalendar, int given
