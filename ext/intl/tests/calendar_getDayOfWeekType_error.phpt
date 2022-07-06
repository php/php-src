--TEST--
IntlCalendar::getDayOfWeekOfType(): bad arguments
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
    var_dump($c->getDayOfWeekType(0));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

try {
    var_dump(intlcal_get_day_of_week_type(1, 1));
} catch (\TypeError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
?>
--EXPECT--
IntlCalendar::getDayOfWeekType(): Argument #1 ($dayOfWeek) must be a valid day of the week
intlcal_get_day_of_week_type(): Argument #1 ($calendar) must be of type IntlCalendar, int given
