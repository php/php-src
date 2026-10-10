--TEST--
Test Csv\LazyLaxCollection::createFromFile(): multibyte enclosure split across the read boundary
--EXTENSIONS--
csv
--FILE--
<?php
$file = __DIR__ . '/enclosure_prefix_chunk_boundary.csv';
/* The enclosure "\nX" starts exactly at the 8192-byte read boundary, so a scan of the
 * first chunk sees only its "\n" prefix, which is also the EOL sequence. */
file_put_contents($file, str_repeat('a', 8190) . ",\nXb\nX\n");

$fromBuffer = Csv\buffer_to_collection_lax(file_get_contents($file), ',', "\nX", "\n");
$fromFile = [];
foreach (Csv\LazyLaxCollection::createFromFile($file, ',', "\nX", "\n") as $row) {
    $fromFile[] = $row;
}
var_dump(count($fromBuffer), $fromFile === $fromBuffer, $fromBuffer[0][1]);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/enclosure_prefix_chunk_boundary.csv');
?>
--EXPECT--
int(1)
bool(true)
string(1) "b"
