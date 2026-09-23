--TEST--
Test Csv\LazyLaxCollection::createFromFile(): final row without a trailing EOL sequence
--EXTENSIONS--
csv
--FILE--
<?php
$file = __DIR__ . '/from_file_no_trailing_eol.csv';
file_put_contents($file, "a,b\r\nc,d");

foreach (Csv\LazyLaxCollection::createFromFile($file) as $row) {
    var_dump($row);
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/from_file_no_trailing_eol.csv');
?>
--EXPECT--
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
