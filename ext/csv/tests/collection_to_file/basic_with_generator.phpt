--TEST--
Test Csv\collection_to_file(): with a Generator collection
--EXTENSIONS--
csv
--FILE--
<?php
$file = __DIR__ . '/collection_to_file_generator.csv';

function rows(): Generator {
    yield ['a', 'b'];
    yield ['c', 'd'];
}

Csv\collection_to_file($file, rows());

echo str_replace("\r\n", "<CRLF>\n", file_get_contents($file));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/collection_to_file_generator.csv');
?>
--EXPECT--
a,b<CRLF>
c,d<CRLF>
