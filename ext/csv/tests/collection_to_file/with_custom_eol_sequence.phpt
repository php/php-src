--TEST--
Test Csv\collection_to_file(): with a custom EOL sequence
--EXTENSIONS--
csv
--FILE--
<?php
$file = __DIR__ . '/collection_to_file_custom_eol.csv';

$collection = [
    ['a', 'b'],
    ['c', 'd'],
];

Csv\collection_to_file($file, $collection, ',', '"', '|EOL|');

var_dump(file_get_contents($file));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/collection_to_file_custom_eol.csv');
?>
--EXPECT--
string(16) "a,b|EOL|c,d|EOL|"
