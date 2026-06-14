--TEST--
IntlCalendar::clear(): bad arguments
--EXTENSIONS--
intl
--FILE--
<?php

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

try {
    var_dump($c->clear(-1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}
try {
    var_dump(intlcal_clear($c, -1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
IntlCalendar::clear(): Argument #1 ($field) must be a valid field
intlcal_clear(): Argument #2 ($field) must be a valid field
