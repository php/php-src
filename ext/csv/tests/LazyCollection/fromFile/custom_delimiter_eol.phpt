--TEST--
Test Csv\LazyLaxCollection::createFromFile(): custom delimiter and multibyte EOL sequence
--EXTENSIONS--
csv
--FILE--
<?php
$file = __DIR__ . '/from_file_custom.csv';
file_put_contents($file, 'a;b|EOL|"c;d";e|EOL|');

foreach (Csv\LazyLaxCollection::createFromFile($file, ';', '"', '|EOL|') as $row) {
    var_dump($row);
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/from_file_custom.csv');
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
  string(3) "c;d"
  [1]=>
  string(1) "e"
}
