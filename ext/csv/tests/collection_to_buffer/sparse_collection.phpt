--TEST--
Test Csv\collection_to_buffer() and Csv\collection_to_file() with a sparse (holey) array
--EXTENSIONS--
csv
--FILE--
<?php
$rows = [['a', 'b'], ['x', 'y'], ['c', 'd']];
unset($rows[1]);
echo str_replace("\r\n", "<CRLF>\n", Csv\collection_to_buffer($rows));

$file = __DIR__ . '/sparse_collection.csv';
Csv\collection_to_file($file, $rows);
echo str_replace("\r\n", "<CRLF>\n", file_get_contents($file));

/* An array that is nothing but holes behaves like an empty collection */
$holes = [['a']];
unset($holes[0]);
var_dump(Csv\collection_to_buffer($holes));
?>
--CLEAN--
<?php
@unlink(__DIR__ . '/sparse_collection.csv');
?>
--EXPECT--
a,b<CRLF>
c,d<CRLF>
a,b<CRLF>
c,d<CRLF>
string(0) ""
