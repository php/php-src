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
    echo $e::class, $e->getMessage(), \PHP_EOL;
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
    echo $e::class, $e->getMessage(), \PHP_EOL;
}

?>
--EXPECTF--
Warning: Array to string conversion in %s on line %d
string(49) "One,Two,Three
Four,Array,Six
Seven,Eight,Nine
"
ErrorObject of class stdClass could not be converted to string
