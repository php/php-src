--TEST--
SplFileObject::fputcsv() with empty $escape
--FILE--
<?php
$data = array(
    ['\\'],
    ['\\"']
);
$file = new SplTempFileObject;
foreach ($data as $record) {
    $file->fputcsv($record, ',', '"', '');
}
$file->rewind();
foreach ($file as $line) {
    echo $line;
}
?>
--EXPECT--
\
"\"""
