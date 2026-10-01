--TEST--
GH-8563 (Different results for seek() on SplFileObject and SplTempFileObject)
--FILE--
<?php
$path = __DIR__ . '/gh8563.txt';
$file_01 = new SplFileObject($path, 'w+');
$file_02 = new SplTempFileObject();

for ($i = 0; $i < 5; $i++) {
    $file_01->fwrite("line {$i}" . PHP_EOL);
    $file_02->fwrite("line {$i}" . PHP_EOL);
}

$file_01->rewind();
$file_02->rewind();

$file_01->seek(10);
$file_02->seek(10);

echo 'SplFileObject: ' . $file_01->key() . "\n";
echo 'SplTempFileObject: ' . $file_02->key() . "\n";
?>
--CLEAN--
<?php
unlink(__DIR__ . '/gh8563.txt');
?>
--EXPECT--
SplFileObject: 5
SplTempFileObject: 5
