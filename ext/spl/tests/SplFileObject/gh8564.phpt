--TEST--
GH-8564 (SplFileObject: next() moves to nonexistent indexes)
--FILE--
<?php
$file = new SplTempFileObject();
for ($i = 0; $i < 5; $i++) {
    $file->fwrite("line {$i}" . PHP_EOL);
}

$file->rewind();
for ($i = 0; $i < 10; $file->next(), $i++);
echo "next() 10x: key=" . $file->key() . " valid=" . var_export($file->valid(), true) . "\n";

$file->rewind();
$file->seek(10);
echo "seek(10): key=" . $file->key() . " valid=" . var_export($file->valid(), true) . "\n";
?>
--EXPECT--
next() 10x: key=5 valid=false
seek(10): key=5 valid=false
