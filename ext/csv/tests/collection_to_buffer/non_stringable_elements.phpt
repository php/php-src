--TEST--
Test Csv\collection_to_buffer() with a row not containing string elements
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

$collection = [
    [
        'One',
        'Two',
        'Three',
    ],
    [
        'Four',
        ['Not a string'],
        'Six',
    ],
    [
        'Seven',
        'Eight',
        'Nine',
    ],
];

try {
    var_dump(Csv\collection_to_buffer($collection));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

$collection = [
    [
        'One',
        'Two',
        'Three',
    ],
    [
        'Four',
        new stdClass(),
        'Six',
    ],
    [
        'Seven',
        'Eight',
        'Nine',
    ],
];

try {
    var_dump(Csv\collection_to_buffer($collection));
} catch (\Throwable $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

?>
--EXPECT--
TypeError: Field 1 of element 1 of the collection must be of type null|bool|int|float|string|BackedEnum, array given
TypeError: Field 1 of element 1 of the collection must be of type null|bool|int|float|string|BackedEnum, stdClass given
