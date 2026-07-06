--TEST--
datefmt_parse() and datefmt_localtime() validate offset type
--EXTENSIONS--
intl
--FILE--
<?php

$fmt = new IntlDateFormatter('en_GB');
$fmt->setPattern('VV');

$offset = 'offset';
try {
    $fmt->parse('America/Los_Angeles', $offset);
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

$offset = 'offset';
try {
    datefmt_parse($fmt, 'America/Los_Angeles', $offset);
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

$offset = 'offset';
try {
    $fmt->localtime('America/Los_Angeles', $offset);
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

$offset = 'offset';
try {
    datefmt_localtime($fmt, 'America/Los_Angeles', $offset);
} catch (TypeError $e) {
    echo $e->getMessage(), PHP_EOL;
}

?>
--EXPECT--
IntlDateFormatter::parse(): Argument #2 ($offset) must be of type int, string given
datefmt_parse(): Argument #3 ($offset) must be of type int, string given
IntlDateFormatter::localtime(): Argument #2 ($offset) must be of type int, string given
datefmt_localtime(): Argument #3 ($offset) must be of type int, string given
