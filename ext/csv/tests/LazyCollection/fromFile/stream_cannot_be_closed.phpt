--TEST--
Test Csv\LazyLaxCollection::createFromFile(): userland cannot close the internal stream
--EXTENSIONS--
csv
--FILE--
<?php
$file = __DIR__ . '/from_file_stream_cannot_be_closed.csv';
file_put_contents($file, "a,b\r\nc,d\r\n");

$collection = Csv\LazyLaxCollection::createFromFile($file);
foreach ($collection as $row) { break; }
/* The internal stream is reachable through the resource list, but closing it is refused,
 * so nothing in userland can free it while the collection still uses it. */
$res = null;
foreach (get_resources('stream') as $candidate) {
    if (stream_get_meta_data($candidate)['uri'] === $file) {
        $res = $candidate;
    }
}
var_dump(fclose($res));
/* Iteration keeps working */
foreach ($collection as $row) {
    echo json_encode($row), \PHP_EOL;
}
/* Destroying the collection closes the stream */
unset($collection);
var_dump(gettype($res));
echo "done", \PHP_EOL;
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/from_file_stream_cannot_be_closed.csv');
?>
--EXPECTF--
Warning: fclose(): cannot close the provided stream, as it must not be manually closed in %s on line %d
bool(false)
["a","b"]
["c","d"]
string(17) "resource (closed)"
done
