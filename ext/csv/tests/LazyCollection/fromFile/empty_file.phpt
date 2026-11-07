--TEST--
Test Csv\LazyLaxCollection::createFromFile(): empty file yields no rows
--EXTENSIONS--
csv
--FILE--
<?php
$file = __DIR__ . '/from_file_empty.csv';
file_put_contents($file, '');

$iterations = 0;
foreach (Csv\LazyLaxCollection::createFromFile($file) as $row) {
    $iterations++;
}
var_dump($iterations);
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/from_file_empty.csv');
?>
--EXPECT--
int(0)
