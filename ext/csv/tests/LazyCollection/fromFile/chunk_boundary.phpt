--TEST--
Test Csv\LazyLaxCollection::createFromFile(): rows spanning read-chunk boundaries match buffer parsing
--EXTENSIONS--
csv
--FILE--
<?php
$file = __DIR__ . '/from_file_chunk_boundary.csv';

/* An enclosed field far larger than the 8KiB read chunk, with embedded EOL sequences,
 * followed by enough small rows to force many buffer compactions. */
$bigField = str_repeat("lorem ipsum\r\n\"quoted\"\r\n", 800);
$rows = [];
$rows[] = ['start', $bigField, 'end'];
for ($i = 0; $i < 10000; $i++) {
    $rows[] = ["field$i", "value,with,commas$i", "text\r\nline$i"];
}
Csv\collection_to_file($file, $rows);

$fromFile = [];
foreach (Csv\LazyLaxCollection::createFromFile($file) as $row) {
    $fromFile[] = $row;
}

$fromBuffer = Csv\buffer_to_collection_lax(file_get_contents($file));

var_dump(count($fromFile));
var_dump($fromFile === $fromBuffer);
var_dump($fromFile[0][1] === $bigField);
echo json_encode($fromFile[10000]), \PHP_EOL;
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/from_file_chunk_boundary.csv');
?>
--EXPECT--
int(10001)
bool(true)
bool(true)
["field9999","value,with,commas9999","text\r\nline9999"]
