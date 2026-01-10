--TEST--
IntlCalendar::isSet(): bad arguments
--EXTENSIONS--
intl
--FILE--
<?php

$c = new IntlGregorianCalendar(NULL, 'pt_PT');

try {
    var_dump($c->isSet(-1));
} catch (\ValueError $e) {
    echo $e->getMessage() . \PHP_EOL;
}

?>
--EXPECT--
IntlCalendar::isSet(): Argument #1 ($field) must be a valid field
