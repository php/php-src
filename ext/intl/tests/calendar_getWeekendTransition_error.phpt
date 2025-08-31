--TEST--
IntlCalendar::getWeekendTransition(): bad arguments
--EXTENSIONS--
intl
--FILE--
<?php

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

try {
    var_dump($c->getWeekendTransition(0));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
IntlCalendar::getWeekendTransition(): Argument #1 ($dayOfWeek) must be a valid day of the week
