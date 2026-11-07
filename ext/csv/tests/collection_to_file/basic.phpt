--TEST--
Test Csv\collection_to_file(): basic behaviour
--EXTENSIONS--
csv
--FILE--
<?php
$file = __DIR__ . '/collection_to_file_basic.csv';

$collection = [
    ['Hello', 'World', 'this'],
    ['is', 'a', 'CSV file'],
    ['with "enclosures"', 'and,delimiters', "and\r\nnew lines"],
];

Csv\collection_to_file($file, $collection);

echo str_replace("\r\n", "<CRLF>\n", file_get_contents($file));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/collection_to_file_basic.csv');
?>
--EXPECT--
Hello,World,this<CRLF>
is,a,CSV file<CRLF>
"with ""enclosures""","and,delimiters","and<CRLF>
new lines"<CRLF>
