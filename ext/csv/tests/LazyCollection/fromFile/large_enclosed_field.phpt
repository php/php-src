--TEST--
Test Csv\LazyLaxCollection::createFromFile(): fields and errors spanning many reads
--DESCRIPTION--
The row boundary scanner resumes where it stopped after each read instead of rescanning the
row from its start, which made a multi-megabyte enclosed field take seconds.
--EXTENSIONS--
csv
--FILE--
<?php
$file = __DIR__ . '/from_file_large_enclosed_field.csv';

$field = str_repeat("x\r\n\"\"y,", 100000);
file_put_contents($file, "\"$field\",b\r\nc,d\r\n");
$rows = iterator_to_array(Csv\LazyLaxCollection::createFromFile($file), false);
var_dump(count($rows), $rows[0][0] === str_replace('""', '"', $field), $rows[0][1], $rows[1]);

/* Same with a multibyte self-overlapping enclosure and multibyte delimiter and EOL */
$field = str_repeat("x--y||z##", 50000) . '-';
file_put_contents($file, Csv\collection_to_buffer([[$field, 'b'], ['c', 'd']], '||', '--', '##'));
$rows = iterator_to_array(Csv\LazyLaxCollection::createFromFile($file, '||', '--', '##'), false);
var_dump($rows === [[$field, 'b'], ['c', 'd']]);

/* An unterminated enclosure at the start of a large file */
file_put_contents($file, '"' . str_repeat("a,b\r\n", 100000));
try {
    foreach (Csv\LazyLaxCollection::createFromFile($file) as $row) {
        echo "unreachable\n";
    }
} catch (\ValueError $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/from_file_large_enclosed_field.csv');
?>
--EXPECT--
int(2)
bool(true)
string(1) "b"
array(2) {
  [0]=>
  string(1) "c"
  [1]=>
  string(1) "d"
}
bool(true)
ValueError: Enclosure sequence is not closed
