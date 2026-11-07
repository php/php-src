--TEST--
Error conditions for Csv\collection_to_buffer()
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
    25,
    [
        'Seven',
        'Eight',
        'Nine',
    ],
];

try {
    var_dump(Csv\collection_to_buffer($collection));
} catch (\TypeError $e) {
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
        'Five',
        'Six'
    ],
    [
        'Seven',
        'Eight',
        'Nine',
    ],
    [
        'Ten',
        'Eleven',
    ],
];

try {
    var_dump(Csv\collection_to_buffer($collection));
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

?>
--EXPECT--
TypeError: Element 1 of the collection must be an array
ValueError: Element 3 of the collection contains 2 fields compared to 3 fields on previous rows
