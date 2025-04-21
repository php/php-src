--TEST--
GH-18348 (fputcsv incorrectly escapes when quote char is both enclosure and escape char)
--FILE--
<?php

$fp = fopen ('php://temp', 'r+');

fputcsv($fp, ['J"a"n'], ',', '"', 'a');
fputcsv($fp, ['He said "Hello"'], ',', '"', '"');
fputcsv($fp, ['Line1\\Line2'], ',', '"', '\\');

rewind($fp);
echo stream_get_contents($fp);
?>
--EXPECT--
"J""a""n"
"He said ""Hello"""
"Line1\Line2"
