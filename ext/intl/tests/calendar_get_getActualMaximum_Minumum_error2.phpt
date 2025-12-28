--TEST--
IntlCalendar::get/getActualMaximum/getActualMinimum(): bad arguments (procedural)
--EXTENSIONS--
intl
--FILE--
<?php

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

try {
    var_dump(intlcal_get($c, -1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(intlcal_get_actual_maximum($c, -1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(intlcal_get_actual_minimum($c, -1));
} catch (Error $e) {
    echo get_class($e) . ': ' . $e->getCode() . ', ' . $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
ValueError: 0, intlcal_get(): Argument #2 ($field) must be a valid field
ValueError: 0, intlcal_get_actual_maximum(): Argument #2 ($field) must be a valid field
ValueError: 0, intlcal_get_actual_minimum(): Argument #2 ($field) must be a valid field
