--TEST--
Test Csv\LazyLaxCollection::createFromFile(): basic behaviour
--EXTENSIONS--
csv
--FILE--
<?php
$file = __DIR__ . '/from_file_basic.csv';
file_put_contents($file, "Hello,World\r\n\"with \"\"quotes\"\"\",\"and\r\nan embedded EOL\"\r\nlast,row\r\n");

$collection = Csv\LazyLaxCollection::createFromFile($file);
var_dump($collection instanceof IteratorAggregate);
foreach ($collection as $index => $row) {
    echo $index, ': ', json_encode($row), \PHP_EOL;
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/from_file_basic.csv');
?>
--EXPECT--
bool(true)
0: ["Hello","World"]
1: ["with \"quotes\"","and\r\nan embedded EOL"]
2: ["last","row"]
