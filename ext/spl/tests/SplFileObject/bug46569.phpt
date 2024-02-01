--TEST--
Bug #46569 (SplFileObject: fgetcsv after seek returns wrong line)
--FILE--
<?php
$file = new SplFileObject(__DIR__ . '/bug46569.csv');
$file->seek(1);
print_r($file->fgetcsv());
?>
--EXPECT--
Array
(
    [0] => second
    [1] => line
)
