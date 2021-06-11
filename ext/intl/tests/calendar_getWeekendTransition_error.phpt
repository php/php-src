--TEST--
IntlCalendar::getWeekendTransition(): bad arguments
--INI--
date.timezone=Atlantic/Azores
--EXTENSIONS--
intl
--FILE--
<?php
ini_set("intl.error_level", E_WARNING);

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

try {
    var_dump($c->getWeekendTransition(0));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(intlcal_get_weekend_transition(1, 1));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
IntlCalendar::getWeekendTransition(): Argument #1 ($dayOfWeek) must be a valid day of the week
intlcal_get_weekend_transition(): Argument #1 ($calendar) must be of type IntlCalendar, int given
