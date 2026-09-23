--TEST--
Test Csv\LazyLaxCollection::createFromFile(): the internal stream is not exposed to userland
--EXTENSIONS--
csv
--FILE--
<?php
$file = __DIR__ . '/from_file_private_stream.csv';
file_put_contents($file, "a,b\r\nc,d\r\n");

$before = count(get_resources('stream'));
$collection = Csv\LazyLaxCollection::createFromFile($file);
foreach ($collection as $row) { break; }
/* The internal stream must not appear in the resource list, so nothing in userland
 * can close it while the collection still uses it. */
var_dump(count(get_resources('stream')) === $before);
$uris = array_map(fn($r) => stream_get_meta_data($r)['uri'] ?? '', get_resources('stream'));
var_dump(in_array($file, $uris, true));
/* Iteration keeps working */
foreach ($collection as $row) {
    echo json_encode($row), \PHP_EOL;
}
unset($collection);
echo "done", \PHP_EOL;
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/from_file_private_stream.csv');
?>
--EXPECT--
bool(true)
bool(false)
["a","b"]
["c","d"]
done
