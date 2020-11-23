--TEST--
IntlCalendar::get/getActualMaximum/getActualMinimum(): bad arguments
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
    var_dump($c->get(-1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump($c->getActualMaximum(-1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump($c->getActualMinimum(-1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
IntlCalendar::get(): Argument #1 ($field) must be a valid field
IntlCalendar::getActualMaximum(): Argument #1 ($field) must be a valid field
IntlCalendar::getActualMinimum(): Argument #1 ($field) must be a valid field
