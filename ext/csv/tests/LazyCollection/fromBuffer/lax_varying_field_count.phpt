--TEST--
Csv\LazyLaxCollection::createFromBuffer() is lax: rows may have a varying number of fields
--SKIPIF--
<?php
if (!extension_loaded('csv')) {
	echo 'skip';
}
?>
--FILE--
<?php

$string = "One,Two,Three\r\nFour\r\nFive,Six\r\nSeven,Eight,Nine,Ten\r\n";

$collection = Csv\LazyLaxCollection::createFromBuffer($string);

var_dump(iterator_to_array($collection) === [
    ['One', 'Two', 'Three'],
    ['Four'],
    ['Five', 'Six'],
    ['Seven', 'Eight', 'Nine', 'Ten'],
]);

try {
    Csv\buffer_to_collection($string);
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}

?>
--EXPECT--
bool(true)
ValueError: Buffer row 2 contains 1 fields compared to 3 fields on previous rows
