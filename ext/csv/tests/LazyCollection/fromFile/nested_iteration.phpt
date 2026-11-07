--TEST--
Test Csv\LazyLaxCollection::createFromFile(): only one loop at a time can read the stream
--EXTENSIONS--
csv
--FILE--
<?php
$file = __DIR__ . '/from_file_nested_iteration.csv';
file_put_contents($file, "a\r\nb\r\n");
$collection = Csv\LazyLaxCollection::createFromFile($file);

foreach ($collection as $outer) {
    try {
        foreach ($collection as $inner) {
            echo "unreachable\n";
        }
    } catch (\Error $e) {
        echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
    }
    echo json_encode($outer), \PHP_EOL;
}

/* Once the first loop is over, the collection can be iterated again */
foreach ($collection as $row) {
    echo json_encode($row), \PHP_EOL;
}
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/from_file_nested_iteration.csv');
?>
--EXPECT--
Error: A Csv\LazyLaxCollection created from a file cannot be iterated by more than one loop at a time
["a"]
Error: A Csv\LazyLaxCollection created from a file cannot be iterated by more than one loop at a time
["b"]
["a"]
["b"]
