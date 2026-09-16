--TEST--
IntlCalendar::get/getActualMaximum/getActualMinimum(): bad arguments
--EXTENSIONS--
intl
--FILE--
<?php

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

try {
    var_dump($c->get(-1));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($c->getActualMaximum(-1));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}
try {
    var_dump($c->getActualMinimum(-1));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
ValueError: IntlCalendar::get(): Argument #1 ($field) must be a valid field
ValueError: IntlCalendar::getActualMaximum(): Argument #1 ($field) must be a valid field
ValueError: IntlCalendar::getActualMinimum(): Argument #1 ($field) must be a valid field
