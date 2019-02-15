--TEST--
fputcsv() with empty $escape
--FILE--
<?php
$data = array(
    ['\\'],
    ['\\"']
);
$stream = fopen('php://memory', 'w+');
foreach ($data as $record) {
    fputcsv($stream, $record, ',', '"', '');
}
rewind($stream);
echo stream_get_contents($stream);
fclose($stream);
?>
===DONE===
--EXPECT--
\
"\"""
===DONE===
