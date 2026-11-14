--TEST--
Test Csv\LazyLaxCollection::createFromFile(): EOL occurring inside a delimiter split at the read boundary
--EXTENSIONS--
csv
--FILE--
<?php
$file = __DIR__ . '/eol_inside_delimiter_chunk_boundary.csv';
/* The delimiter "x\ny" contains the EOL "\n" in its interior and is placed so the first
 * 8192-byte read ends right after that "\n": a scanner without interior-token lookahead
 * would commit a bogus row boundary there. */
file_put_contents($file, str_repeat('a', 8190) . "x\nyb\n");

$fromBuffer = Csv\buffer_to_collection_lax(file_get_contents($file), "x\ny", '"', "\n");
$fromFile = [];
foreach (Csv\LazyLaxCollection::createFromFile($file, "x\ny", '"', "\n") as $row) {
    $fromFile[] = $row;
}
var_dump(count($fromBuffer), count($fromBuffer[0]), $fromFile === $fromBuffer, $fromBuffer[0][1]);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/eol_inside_delimiter_chunk_boundary.csv');
?>
--EXPECT--
int(1)
int(2)
bool(true)
string(1) "b"
