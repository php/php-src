--TEST--
Test Csv\LazyLaxCollection::createFromFile(): iterating twice, including after an early break
--EXTENSIONS--
csv
--FILE--
<?php
$file = __DIR__ . '/from_file_rewind.csv';
file_put_contents($file, "a,b\r\nc,d\r\ne,f\r\n");

$collection = Csv\LazyLaxCollection::createFromFile($file);

/* Partial iteration */
foreach ($collection as $row) {
    var_dump($row);
    break;
}
echo "--- second run ---", \PHP_EOL;
/* Full iteration from the start again */
foreach ($collection as $row) {
    var_dump($row);
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/from_file_rewind.csv');
?>
--EXPECT--
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
--- second run ---
array(2) {
  [0]=>
  string(1) "a"
  [1]=>
  string(1) "b"
}
array(2) {
  [0]=>
  string(1) "c"
  [1]=>
  string(1) "d"
}
array(2) {
  [0]=>
  string(1) "e"
  [1]=>
  string(1) "f"
}
