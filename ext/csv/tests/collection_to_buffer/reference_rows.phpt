--TEST--
Test Csv\collection_to_buffer() and Csv\collection_to_file() with rows that are references
--EXTENSIONS--
csv
--FILE--
<?php
$row1 = ['a', 'b'];
$row2 = ['c', 'd'];
$collection = [&$row1, &$row2];
echo json_encode(Csv\collection_to_buffer($collection)), \PHP_EOL;

$file = __DIR__ . '/reference_rows.csv';
Csv\collection_to_file($file, $collection);
echo json_encode(file_get_contents($file)), \PHP_EOL;

/* A field that is a reference */
$field = 'x';
echo json_encode(Csv\collection_to_buffer([[&$field, 'y']])), \PHP_EOL;

/* A reference to something that is not an array is still rejected */
$notArray = 'a';
try {
    Csv\collection_to_buffer([&$notArray]);
} catch (\TypeError $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/reference_rows.csv');
?>
--EXPECT--
"a,b\r\nc,d\r\n"
"a,b\r\nc,d\r\n"
"x,y\r\n"
TypeError: Element 0 of the collection must be an array
