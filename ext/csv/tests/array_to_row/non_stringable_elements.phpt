--TEST--
Test Csv\array_to_row() with an array not containing string elements
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

$fields = [
    'Hello',
    'Field has spaces',
    ['Not a string'],
    'Has field escape sequence " inside field',
    'Basic',
];

try {
    var_dump(Csv\array_to_row($fields));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

$fields = [
    'Hello',
    'Field has spaces',
    new stdClass(),
    'Has field escape sequence " inside field',
    'Basic',
];

try {
    var_dump(Csv\array_to_row($fields));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

?>
--EXPECT--
TypeError: Field 2 must be of type null|bool|int|float|string|BackedEnum, array given
TypeError: Field 2 must be of type null|bool|int|float|string|BackedEnum, stdClass given
