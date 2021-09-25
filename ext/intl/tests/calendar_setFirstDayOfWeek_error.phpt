--TEST--
IntlCalendar::setFirstDayOfWeek(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--FILE--
<?php

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

try {
    var_dump($c->setFirstDayOfWeek(0));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(intlcal_set_first_day_of_week($c, 0));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(intlcal_set_first_day_of_week(1, 2));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
IntlCalendar::setFirstDayOfWeek(): Argument #1 ($dayOfWeek) must be a valid day of the week
intlcal_set_first_day_of_week(): Argument #2 ($dayOfWeek) must be a valid day of the week
intlcal_set_first_day_of_week(): Argument #1 ($calendar) must be of type IntlCalendar, int given
