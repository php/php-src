--TEST--
Test Csv\LazyLaxCollection::createFromFile(): nonexistent file throws
--EXTENSIONS--
csv
--FILE--
<?php
try {
    Csv\LazyLaxCollection::createFromFile(__DIR__ . '/does_not_exist.csv');
} catch (Error $e) {
    echo $e::class, ': ', $e->getMessage(), \PHP_EOL;
}
?>
--EXPECTF--
Error: Failed to open "%sdoes_not_exist.csv" for reading
