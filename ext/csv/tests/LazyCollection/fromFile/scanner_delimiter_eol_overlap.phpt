--TEST--
Test Csv\LazyLaxCollection::createFromFile(): EOL being a prefix of the delimiter must not split rows
--EXTENSIONS--
csv
--FILE--
<?php
$file = __DIR__ . '/scanner_delimiter_eol_overlap.csv';
file_put_contents($file, 'a||b|');

$fromBuffer = Csv\buffer_to_collection_lax(file_get_contents($file), '||', '"', '|');
$fromFile = [];
foreach (Csv\LazyLaxCollection::createFromFile($file, '||', '"', '|') as $row) {
    $fromFile[] = $row;
}
echo json_encode($fromBuffer), \PHP_EOL;
var_dump($fromFile === $fromBuffer);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/scanner_delimiter_eol_overlap.csv');
?>
--EXPECT--
[["a","b"]]
bool(true)
